// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/uint32_divisor.h"

#include <stdexcept>

namespace toft {

UInt32Divisor::UInt32Divisor() :
    m_divisor(0),
    m_algorithm(Algorithm_Invalid),
    m_multiplier(0),
    m_shift(0)
{
}

UInt32Divisor::UInt32Divisor(uint32_t divisor)
{
    if (!SetValue(divisor))
        throw std::runtime_error("can't find mul and shift");
}

/// Set new divisor, then update internal state.
/// @return whether success
/// @return true success
/// @false invalid divisor, object keep unchanged
bool UInt32Divisor::SetValue(uint32_t divisor)
{
    if (DetectAlgorithm(divisor, &m_algorithm, &m_multiplier, &m_shift))
    {
        m_divisor = divisor;
        return true;
    }
    return false;
}

static uint32_t log2(uint32_t i)
{
    uint32_t t = 0;
    i = i >> 1;
    while (i)
    {
        i = i >> 1;
        t++;
    }
    return t;
}

/// Detect algorithm and corresponding parameters.
/// the following code is from:
/// [2] AMD Athlon Processor x86 Code Optimization Guide, page 144,
/// and made some necessary modification.
/// @param divisor divisor to be handled
/// @param multiply multiply factor
/// @param shift shift factor (0..63)
/// @retval true if a valid mul and shift value are found.
bool UInt32Divisor::DetectAlgorithm(
    uint32_t divisor,
    UInt32Divisor::Algorithm* algorithm,
    uint32_t* multiplier,
    uint32_t* shift)
{
    if (divisor == 0)
        return false;

    if (divisor >= 0x80000000) {
        *algorithm = Algorithm_Compare;
        return true;
    }

    /* Reduce divisor until it becomes odd */
    uint32_t lowest_zero_bits = 0;
    uint32_t t = divisor;
    while (!(t & 1))
    {
        t >>= 1;
        lowest_zero_bits++;
    }

    // is power of 2
    if (t == 1)
    {
        *algorithm = Algorithm_Shift;
        *multiplier = 1;
        *shift = lowest_zero_bits;
        return true;
    }

    /* Generate multiplier, shift for algorithm 0. Based on: Granlund, T.;
     * Montgomery,
     * P.L.: "Division by Invariant Integers using Multiplication".
     * SIGPLAN Notices, Vol. 29, June 1994, page 61.
     * */

    uint32_t l = log2(t) + 1;
    uint64_t j = ((0xffffffffULL) % (uint64_t) t);
    uint64_t k = (1ULL << (32 + l)) / (uint64_t) (0xffffffff - j);
    uint64_t m_low = (1ULL << (32 + l)) / t;
    uint64_t m_high = ((1ULL << (32 + l)) + k) / t;

    while (((m_low >> 1) < (m_high >> 1)) && (l > 0))
    {
        m_low = m_low >> 1;
        m_high = m_high >> 1;
        --l;
    }

    if ((m_high >> 32) == 0)
    {
        *multiplier = (uint32_t) m_high;
        *shift = l;
        *algorithm = Algorithm_MultipleShift;
    }
    else
    {
        /* Generate multiplier, shift for algorithm 1. Based on: Magenheimer,
         * D.J.; et al:
         * "Integer Multiplication and Division on the HP Precision Architecture".
         * IEEE Transactions on Computers, Vol 37, No. 8, August 1988, page 980.
         * */
        *shift = log2(t);
        m_low = (1ULL << (32 + *shift)) / (uint64_t) t;
        uint32_t r = (uint32_t) ((1ULL << (32 + *shift)) % (uint64_t) t);
        *multiplier = (r < ((t>>1)+1)) ? (uint32_t) m_low : (uint32_t) m_low + 1;
        *algorithm = Algorithm_MultipleAddShift;
    }

    /* Reduce multiplier for either algorithm to smallest possible */
    while (!(*multiplier & 1))
    {
        *multiplier >>= 1;
        --*shift;
    }

    /* Adjust multiplier for reduction of even divisors */
    *shift += lowest_zero_bits;
    return true;
}

} // namespace toft
