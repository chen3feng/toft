// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_UINT32_DIVISOR_H
#define TOFT_BASE_UINT32_DIVISOR_H

#include <assert.h>
#include <stdint.h>

namespace toft {

/// Optimized unsigned 32 bit integer divide for invariant divisors.
/// majorly using multiply and shift to instead of CPU div instruction.
/// performance:
/// 4 times faster than div instruction for small (< 10000) divisors.
/// 3 times faster than div instruction for midium (10000, 10000000) divisors.
/// 2 times faster than div instruction for large (> 10000000) divisors.
/// @sa
/// [1] http://www.hackersdelight.org/divcMore.pdf
/// [2] http://support.amd.com/us/Processor_TechDocs/22007.pdf
/// @note dividend can't be 64 bits
class UInt32Divisor
{
    // objects of this class are safe to copy and assign.
public:
    enum Algorithm
    {
        Algorithm_MultipleShift,
        Algorithm_MultipleAddShift,
        Algorithm_Shift,
        Algorithm_Compare,
        Algorithm_Invalid = -1
    };
public:
    UInt32Divisor();
    explicit UInt32Divisor(uint32_t divisor);

    /// Set new divisor, then update internal state
    /// @return whether success
    /// @return true success
    /// @false invalid divisor, object keep unchanged
    bool SetValue(uint32_t divisor);

    /// Do divide operation
    /// @return the quotient
    uint32_t Divide(uint32_t dividend) const
    {
        switch (m_algorithm)
        {
        case Algorithm_MultipleShift:
            {
                uint64_t product = (uint64_t) dividend * (uint64_t) m_multiplier;
                uint32_t high = product >> 32;
                return high >> m_shift;
            }
        case Algorithm_MultipleAddShift:
            {
                uint64_t product = (uint64_t) dividend * (uint64_t) m_multiplier;
                uint32_t high = product >> 32;
                uint32_t low = (uint32_t) product;
                if (low + m_multiplier < low) // check overflow
                    ++high;
                return high >> m_shift;
            }
        case Algorithm_Shift:
            return dividend >> m_shift;
        case Algorithm_Compare:
            return dividend >= m_divisor;
        case Algorithm_Invalid:
            assert(!"Invalid divisor");
        }

        // never reachable
        return 0;
    }

    /// Do divide operation, obtain both quotient and reminder
    /// @return quotient
    uint32_t Divide(uint32_t dividend, uint32_t* remainder) const
    {
        uint32_t quotient = Divide(dividend);
        *remainder = dividend - quotient * m_divisor;
        return quotient;
    }

    /// Mudulu operation
    /// @return remainder
    uint32_t Modulu(uint32_t dividend) const
    {
        return dividend - Divide(dividend) * m_divisor;
    }

private:
    static bool DetectAlgorithm(
        uint32_t divisor,
        Algorithm* algorithm,
        uint32_t* multiplier,
        uint32_t* shift
    );

public: // attributes
    uint32_t GetValue() const
    {
        return m_divisor;
    }

    // Just for test
    Algorithm GetAlgorithm() const
    {
        return m_algorithm;
    }

    // Just for test
    uint32_t GetMultiplier() const
    {
        return m_multiplier;
    }

    // Just for test
    uint32_t GetShift() const
    {
        return m_shift;
    }

private:
    uint32_t m_divisor;
    Algorithm m_algorithm;
    uint32_t m_multiplier;
    uint32_t m_shift;
};

} // namespace toft

#endif // TOFT_BASE_UINT32_DIVISOR_H
