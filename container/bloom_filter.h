// copyright (c) 2013, the toft authors.
// all rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_CONTAINER_BLOOM_FILTER_H
#define TOFT_CONTAINER_BLOOM_FILTER_H

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <string>
#include <vector>

#include "toft/base/uint32_divisor.h"

// GLOBAL_NOLINT(runtime/int)

namespace toft {

/**
 * A space-efficent probabilistic set for membership test, false postives
 * are possible, but false negatives are not.
 */
class BloomFilter
{
public:
    /// Default ctor, set bloom filter to uninitialized state
    BloomFilter();

    /// @param element_count max optimized element count
    /// @param false_positive_prob false positive prob when reach max element count
    BloomFilter(size_t element_count, double false_positive_prob);

    /// @param bitmap_byte_size bitmap byte size
    /// @param num_hashes number of hash functions
    BloomFilter(size_t bitmap_byte_size, size_t num_hashes);

    /// @param bitmap extsited bitmap
    /// @param bitmap_byte_size bitmap byte size
    /// @param num_hashes number of hash functions
    /// @param copy whether copy bitmap
    BloomFilter(void* bitmap, size_t bitmap_byte_size, size_t num_hashes,
                bool copy = true);

    ~BloomFilter();

    /// @param bitmap extsited bitmap
    /// @param bitmap_byte_size bitmap byte size
    /// @param num_hashes number of hash functions
    /// @param copy whether copy bitmap
    void Initialize(void* bitmap, size_t bitmap_byte_size, int num_hashes, bool copy = true);

    /// @param bitmap_byte_size bitmap byte size
    /// @param num_hashes number of hash functions
    void Initialize(size_t bitmap_byte_size, size_t num_hashes);

    /// @param element_count max optimized element count
    /// @param false_positive_prob false positive prob when reach max element count
    void Initialize(size_t element_count, double false_positive_prob);

    /// Destroy the bloom filter and free all allocated resources
    void Destroy();

    /// Insert a key
    void Insert(const void *key, size_t len);

    /// Insert a c string
    void Insert(const char* key)
    {
        Insert(key, strlen(key));
    }

    /// Insert a c++ string
    void Insert(const std::string& key)
    {
        Insert(key.data(), key.size());
    }

    /// Try insert an unique key and return previous status
    /// @retval true key doesn't exist before insert
    /// @retval false key exist or false positive (conflict) before insert
    bool InsertUnique(const void *key, size_t len);

    bool InsertUnique(const char* key)
    {
        return InsertUnique(key, strlen(key));
    }

    bool InsertUnique(const std::string& key)
    {
        return InsertUnique(key.data(), key.size());
    }

    /// Clear all keys
    void Clear()
    {
        memset(m_bitmap, 0, MemorySize());
    }

    /// @return possible existance of key
    bool MayContain(const void *key, size_t len) const;

    /// @return possible existance of key
    bool MayContain(const std::string& key) const
    {
        return MayContain(key.data(), key.size());
    }

    /// @return possible existance of key
    bool MayContain(const char* key) const
    {
        return MayContain(key, strlen(key));
    }

    /// Is correct initialized
    bool IsValid() const
    {
        return m_bitmap != NULL;
    }

    /// Total bit count
    uint64_t TotalBits() const
    {
        assert(IsValid());
        return m_num_bits;
    }

    /// Total memory used, in bytes
    size_t MemorySize() const;

    /// @return number of hash functions
    unsigned int HashNumber() const
    {
        assert(IsValid());
        return m_num_hash_functions;
    }

    bool IsOwnBitmap()
    {
        return m_own_bitmap;
    }

    unsigned char* GetBitmap()
    {
        return m_bitmap;
    }
    const unsigned char* GetBitmap() const
    {
        return m_bitmap;
    }

    uint64_t Capacity() const
    {
        assert(IsValid());
        return m_element_count;
    }

    double FalsePositiveProb() const
    {
        assert(IsValid());
        return m_false_positive_prob;
    }

private:
    /// helper function used by ctors to initialize all members
    void InitialClear();
    void UncheckedInitialize(
        void* bitmap,
        size_t bitmap_byte_size,
        size_t num_hashes);
    void UncheckedInitialize(size_t bitmap_byte_size, size_t num_hashes);
    static void CheckBitmapSize(size_t byte_size);

private:
    BloomFilter(const BloomFilter&);
    BloomFilter& operator=(const BloomFilter&);

private:
    size_t     m_element_count;
    double     m_false_positive_prob;
    size_t     m_num_hash_functions;
    uint8_t*   m_bitmap;
    uint64_t m_num_bits;
    UInt32Divisor m_divisor;
    bool m_own_bitmap; ///< whether we own the bitmap
};

class PartialBloomFilter
{
public:
    PartialBloomFilter(size_t element_count, double false_positive_prob);
    ~PartialBloomFilter();

    /* XXX/review static functions to expose the bloom filter parameters, given
       1) probablility and # keys
       2) #keys and fix the total size (m), to calculate the optimal
       probability - # hash functions to use
       */
    void Insert(const void *key, size_t len);

    void Insert(const char* key)
    {
        Insert(key, strlen(key));
    }

    void Insert(const std::string& key)
    {
        Insert(key.data(), key.size());
    }

    bool MayContain(const void *key, size_t len) const;

    bool MayContain(const std::string& key) const
    {
        return MayContain(key.data(), key.size());
    }

    bool MayContain(const char* key) const
    {
        return MayContain(key, strlen(key));
    }

    size_t MemorySize() const;

    unsigned int HashNumber() const
    {
        return m_num_hash_functions;
    }

private:
    void Initialize(size_t element_count, double false_positive_prob);

private:
    size_t     m_element_count;
    double     m_false_positive_prob;
    size_t     m_num_hash_functions;
    uint64_t m_num_bits;
    std::vector<uint8_t*>   m_bitmaps;
};

} // namespace toft

#endif // TOFT_CONTAINER_BLOOM_FILTER_H
