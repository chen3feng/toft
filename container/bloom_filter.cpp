// copyright (c) 2013, the toft authors.
// all rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/container/bloom_filter.h"

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <exception>
#include <stdexcept>

#include "toft/base/string/format.h"
#include "toft/hash/murmur.h"

namespace toft {

#if defined __i386__ || defined __x86_64__
/**
 * __test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static bool x86_test_and_set_bit(int nr, volatile uint32_t * addr)
{
    bool oldbit;

    __asm__(
        "btsl %2,%1\n\tsetc %0"
        :"=q" (oldbit), "+m" (*addr)
        :"Ir" (nr));
    return oldbit;
}
#endif

//////////////////////////////////////////////////////////////////////////////
// BloomFilter members

BloomFilter::BloomFilter()
{
    InitialClear();
}

BloomFilter::BloomFilter(size_t element_count, double false_positive_prob)
{
    InitialClear();
    Initialize(element_count, false_positive_prob);
}

BloomFilter::BloomFilter(size_t bitmap_byte_size, size_t num_hashes)
{
    InitialClear();
    Initialize(bitmap_byte_size, num_hashes);
}

BloomFilter::BloomFilter(void* bitmap, size_t bitmap_byte_size,
                         size_t num_hashes, bool copy)
{
    InitialClear();
    Initialize(bitmap, bitmap_byte_size, num_hashes, copy);
}

BloomFilter::~BloomFilter()
{
    Destroy();
}

void BloomFilter::Initialize(size_t bitmap_byte_size, size_t num_hashes)
{
    Destroy();
    CheckBitmapSize(bitmap_byte_size);
    UncheckedInitialize(bitmap_byte_size, num_hashes);
}

void BloomFilter::Initialize(size_t element_count, double false_positive_prob)
{
    double num_hashes = -log(false_positive_prob) / log(2.0);
    size_t num_hash_functions = (size_t) ceil(num_hashes + 0.001);
    uint64_t num_bits =
        static_cast<uint64_t>(element_count * num_hash_functions / log(2.0));

    // round up
    uint64_t num_bytes = (num_bits + CHAR_BIT - 1) / CHAR_BIT;
    num_bits = num_bytes * CHAR_BIT; // then in CHAR_BIT times

    if (num_bits == 0)
    {
        throw std::runtime_error(StringPrint("Num elements=%u false_positive_prob=%g",
                                             element_count, false_positive_prob));
    }

    if (num_bytes > ~size_t(0))
    {
        throw std::runtime_error(
            StringPrint("Bitmap too large, size=%u, exceed size_t limitation",
                        num_bytes));
    }

    Initialize((size_t) num_bytes, num_hash_functions);

    // m_element_count = element_count;
    // m_false_positive_prob = false_positive_prob;
}

void BloomFilter::Initialize(void* bitmap, size_t bitmap_byte_size,
                             int num_hashes, bool copy)
{
    Destroy();
    CheckBitmapSize(bitmap_byte_size);
    if (copy)
    {
        void* bitmap_copy = calloc(bitmap_byte_size, 1);
        if (!bitmap_copy)
            throw std::bad_alloc();
        memcpy(bitmap_copy, bitmap, bitmap_byte_size);
        bitmap = bitmap_copy;
    }
    UncheckedInitialize(bitmap, bitmap_byte_size, num_hashes);
    m_own_bitmap = copy;
}

void BloomFilter::InitialClear()
{
    m_element_count = 0;
    m_false_positive_prob = 1.0;
    m_num_hash_functions = 0;
    m_bitmap = NULL;
    m_num_bits = 0;
    m_own_bitmap = false;
}

void BloomFilter::UncheckedInitialize(
    void* bitmap,
    size_t bitmap_byte_size,
    size_t num_hashes)
{
    m_num_hash_functions = num_hashes;
    m_bitmap = reinterpret_cast<uint8_t*>(bitmap);
    uint64_t num_bits = static_cast<uint64_t>(bitmap_byte_size) * CHAR_BIT;
    m_num_bits = num_bits;
    if (!m_divisor.SetValue(m_num_bits))
        abort();
    m_false_positive_prob = exp(-log(2.0) * num_hashes);
    m_element_count = static_cast<size_t>(
        num_bits * log(2.0) / m_num_hash_functions
    );
}

void BloomFilter::UncheckedInitialize(size_t bitmap_byte_size, size_t num_hashes)
{
    void* bitmap = calloc(bitmap_byte_size, 1);
    if (!bitmap)
        throw std::bad_alloc();
    UncheckedInitialize(bitmap, bitmap_byte_size, num_hashes);
    m_own_bitmap = true;
}

void BloomFilter::Destroy()
{
    if (m_own_bitmap)
    {
        free(m_bitmap);
    }
    InitialClear();
}

void BloomFilter::CheckBitmapSize(size_t byte_size)
{
    uint64_t num_bits = static_cast<uint64_t>(byte_size) * CHAR_BIT;
    if (num_bits >= UINT32_MAX)
    {
        throw std::runtime_error(StringPrint(
                "Bitmap too large, bit count=%u, exceed 4Gbits(512 Mbytes)",
                num_bits));
    }
}

size_t BloomFilter::MemorySize() const
{
    assert(IsValid());
    return (size_t)((m_num_bits + CHAR_BIT - 1) / CHAR_BIT);
}

void BloomFilter::Insert(const void *key, size_t len)
{
    uint32_t digest[4];
    for (size_t i = 0; i < m_num_hash_functions; ++i)
    {
        if (i % 4 == 0)
        {
            if (i != 0) // use original key at first time
                MurmurHash3_x86_128(digest, 16, i, digest);
            else
                MurmurHash3_x86_128(key, len, 0, digest);
        }

        uint32_t bit_index = m_divisor.Modulu(digest[i % 4]);
        assert(bit_index == digest[i % 4] % m_num_bits);
        m_bitmap[bit_index / CHAR_BIT] |= (1 << (bit_index % CHAR_BIT));
    }
}

bool BloomFilter::InsertUnique(const void *key, size_t len)
{
    unsigned int exist_count = 0;
    uint32_t digest[4];
    for (size_t i = 0; i < m_num_hash_functions; ++i)
    {
        if (i % 4 == 0)
        {
            if (i != 0)
                MurmurHash3_x86_128(digest, 16, i, digest);
            else // use original key at first time
                MurmurHash3_x86_128(key, len, 0, digest);
        }

        uint32_t bit_index = m_divisor.Modulu(digest[i % 4]);
        assert(bit_index == digest[i % 4] % m_num_bits);
#if defined __i386__ || defined __x86_64__
        exist_count += x86_test_and_set_bit(
            bit_index,
            reinterpret_cast<uint32_t*>(m_bitmap)
        );
#else
        uint8_t mask = (1 << (bit_index % CHAR_BIT));
        exist_count += (m_bitmap[bit_index / CHAR_BIT] & mask) != 0;
        m_bitmap[bit_index / CHAR_BIT] |= mask;
#endif
    }
    return exist_count < m_num_hash_functions;
}

/// @return possible existance of key
bool BloomFilter::MayContain(const void *key, size_t len) const
{
    uint32_t digest[4];
    for (size_t i = 0; i < m_num_hash_functions; ++i)
    {
        if (i % 4 == 0)
        {
            if (i != 0) // use original key at first time
                MurmurHash3_x86_128(digest, 16, i, digest);
            else
                MurmurHash3_x86_128(key, len, 0, digest);
        }

        uint32_t bit_index = m_divisor.Modulu(digest[i % 4]);
        assert(bit_index == digest[i % 4] % m_num_bits);
        uint8_t byte = m_bitmap[bit_index / CHAR_BIT];
        uint8_t mask = (1 << (bit_index % CHAR_BIT));

        if ( (byte & mask) == 0 )
        {
            return false;
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// PartialBloomFilter members

PartialBloomFilter::PartialBloomFilter(size_t element_count,
                                       double false_positive_prob)
{
    Initialize(element_count, false_positive_prob);
}

PartialBloomFilter::~PartialBloomFilter()
{
    for (size_t i = 0; i < m_bitmaps.size(); ++i)
        free(m_bitmaps[i]);
}

void PartialBloomFilter::Initialize(size_t element_count, double false_positive_prob)
{
    m_element_count = element_count;
    m_false_positive_prob = false_positive_prob;
    double num_hashes = -log(m_false_positive_prob) / log(2);
    m_num_hash_functions = (size_t)(num_hashes + 0.5);
    uint64_t total_bits =
        static_cast<uint64_t>(m_element_count * num_hashes / log(2));
    if (total_bits == 0)
    {
        throw std::runtime_error(StringPrint("Num elements=%u false_positive_prob=%g",
                                             element_count,
                                             false_positive_prob));
    }
    uint64_t single_bits = static_cast<uint64_t>(total_bits / num_hashes);

    if (single_bits >= UINT32_MAX)
    {
        throw std::runtime_error(
            StringPrint("Bitmap too large, bit count=%u, exceed 4Gbits(512 Mbytes)",
                        single_bits));
    }

    m_num_bits = single_bits;
    m_bitmaps.resize(m_num_hash_functions);

    uint64_t num_bytes = (single_bits + CHAR_BIT - 1) / CHAR_BIT;
    if (num_bytes > ~size_t(0))
    {
        throw std::runtime_error(
            StringPrint("Bitmap too large, size=%u, exceed size_t limitation",
                        num_bytes
            )
        );
    }

    for (size_t i = 0; i < m_num_hash_functions; ++i)
    {
        m_bitmaps[i] = static_cast<uint8_t*>(calloc(num_bytes, 1));
        if (!m_bitmaps[i])
        {
            for (size_t j = 0; j < i; ++j)
                free(m_bitmaps[j]);
            throw std::bad_alloc();
        }
    }
}

void PartialBloomFilter::Insert(const void *key, size_t len)
{
    uint32_t hash = 0;
    for (size_t i = 0; i < m_num_hash_functions; ++i)
    {
        hash = MurmurHash2A(key, len, hash);
        uint32_t bit_index = hash % m_num_bits;
        m_bitmaps[i][bit_index / CHAR_BIT] |= (1 << (bit_index % CHAR_BIT));
    }
}

bool PartialBloomFilter::MayContain(const void *key, size_t len) const
{
    uint32_t hash = 0;

    for (size_t i = 0; i < m_num_hash_functions; ++i)
    {
        hash = MurmurHash2A(key, len, hash);

        uint32_t bit_index = hash % m_num_bits;
        uint8_t byte = m_bitmaps[i][bit_index / CHAR_BIT];
        uint8_t byte_mask = uint8_t(1 << (bit_index % CHAR_BIT));

        if ( (byte & byte_mask) == 0 )
        {
            return false;
        }
    }
    return true;
}

size_t PartialBloomFilter::MemorySize() const
{
    return m_num_hash_functions * (m_num_bits + CHAR_BIT - 1) / CHAR_BIT;
}

} // namespace toft

