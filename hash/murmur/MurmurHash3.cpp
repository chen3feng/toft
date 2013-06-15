//  GLOBLA_NOLINT(legal/copyright)
//  GLOBAL_NOLINT(readability/casting)

#include <stdlib.h>    // for _rotl

#include "toft/hash/murmur.h"

namespace toft {

#ifdef __GNUC__
// implement _rotl rotl64 of MSC
inline uint32_t _rotl(uint32_t n, int c)
{
    return (uint32_t) ((n << c) | (n >> (32 - c)));
}

inline uint64_t _rotl64(uint64_t n, int c)
{
    return (uint64_t) ((n << c) | (n >> (64 - c)));
}
#endif

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

inline uint32_t getblock(const uint32_t * p, int i)
{
    return p[i];
}

//----------
// Finalization mix - force all bits of a hash block to avalanche

// avalanches all bits to within 0.25% bias

inline uint32_t fmix32(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}

inline void bmix32(uint32_t & h1, uint32_t & k1, uint32_t & c1, uint32_t & c2)
{
    k1 *= c1;
    k1  = _rotl(k1, 11);
    k1 *= c2;
    h1 ^= k1;

    h1 = h1*3+0x52dce729;

    c1 = c1*5+0x7b7d159c;
    c2 = c2*5+0x6bce6396;
}

//----------

void MurmurHash3_x86_32(const void * key, int len, uint32_t seed, void * out)
{
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 4;

    uint32_t h1 = 0x971e137b ^ seed;

    uint32_t c1 = 0x95543787;
    uint32_t c2 = 0x2ad7eb25;

    //----------
    // body

    const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

    for (int i = -nblocks; i; i++)
    {
        uint32_t k1 = getblock(blocks, i);
        bmix32(h1, k1, c1, c2);
    }

    //----------
    // tail

    const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

    uint32_t k1 = 0;

    switch (len & 3)
    {
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1: k1 ^= tail[0];
            bmix32(h1, k1, c1, c2);
    };

    //----------
    // finalization

    h1 ^= len;

    h1 = fmix32(h1);

    *(uint32_t*)out = h1;
}

//-----------------------------------------------------------------------------

inline void bmix32(uint32_t & h1, uint32_t & h2, uint32_t & k1, uint32_t & k2,
                   uint32_t & c1, uint32_t & c2)
{
    k1 *= c1;
    k1  = _rotl(k1, 11);
    k1 *= c2;
    h1 ^= k1;
    h1 += h2;

    h2 = _rotl(h2, 17);

    k2 *= c2;
    k2  = _rotl(k2, 11);
    k2 *= c1;
    h2 ^= k2;
    h2 += h1;

    h1 = h1*3+0x52dce729;
    h2 = h2*3+0x38495ab5;

    c1 = c1*5+0x7b7d159c;
    c2 = c2*5+0x6bce6396;
}

//----------

void MurmurHash3_x86_64(const void * key, const int len, const uint32_t seed, void * out)
{
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 8;

    uint32_t h1 = 0x8de1c3ac ^ seed;
    uint32_t h2 = 0xbab98226 ^ seed;

    uint32_t c1 = 0x95543787;
    uint32_t c2 = 0x2ad7eb25;

    //----------
    // body

    const uint32_t * blocks = (const uint32_t *)(data + nblocks*8);

    for (int i = -nblocks; i; i++)
    {
        uint32_t k1 = getblock(blocks, i * 2 + 0);
        uint32_t k2 = getblock(blocks, i * 2 + 1);
        bmix32(h1, h2, k1, k2, c1, c2);
    }

    //----------
    // tail

    const uint8_t * tail = (const uint8_t*)(data + nblocks*8);

    uint32_t k1 = 0;
    uint32_t k2 = 0;

    switch (len & 7)
    {
    case 7: k2 ^= tail[6] << 16;
    case 6: k2 ^= tail[5] << 8;
    case 5: k2 ^= tail[4] << 0;
    case 4: k1 ^= tail[3] << 24;
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1: k1 ^= tail[0] << 0;
            bmix32(h1, h2, k1, k2, c1, c2);
    };

    //----------
    // finalization

    h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 = fmix32(h1);
    h2 = fmix32(h2);

    h1 += h2;
    h2 += h1;

    ((uint32_t*)out)[0] = h1;
    ((uint32_t*)out)[1] = h2;
}

//-----------------------------------------------------------------------------
// This mix is large enough that VC++ refuses to inline it unless we use
// __forceinline. It's also not all that fast due to register spillage.

inline void bmix32(uint32_t & h1, uint32_t & h2, uint32_t & h3, uint32_t & h4,
                   uint32_t & k1, uint32_t & k2, uint32_t & k3, uint32_t & k4,
                   uint32_t & c1, uint32_t & c2)
{
    k1 *= c1;
    k1  = _rotl(k1, 11);
    k1 *= c2;
    h1 ^= k1;
    h1 += h2;
    h1 += h3;
    h1 += h4;

    h1 = _rotl(h1, 17);

    k2 *= c2;
    k2  = _rotl(k2, 11);
    k2 *= c1;
    h2 ^= k2;
    h2 += h1;

    h1 = h1 * 3 + 0x52dce729;
    h2 = h2 * 3 + 0x38495ab5;

    c1 = c1 * 5 + 0x7b7d159c;
    c2 = c2 * 5 + 0x6bce6396;

    k3 *= c1;
    k3  = _rotl(k3, 11);
    k3 *= c2;
    h3 ^= k3;
    h3 += h1;

    k4 *= c2;
    k4  = _rotl(k4, 11);
    k4 *= c1;
    h4 ^= k4;
    h4 += h1;

    h3 = h3*3+0x52dce729;
    h4 = h4*3+0x38495ab5;

    c1 = c1*5+0x7b7d159c;
    c2 = c2*5+0x6bce6396;
}

//----------

void MurmurHash3_x86_128(const void * key, const int len, const uint32_t seed, void * out)
{
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 16;

    uint32_t h1 = 0x8de1c3ac ^ seed;
    uint32_t h2 = 0xbab98226 ^ seed;
    uint32_t h3 = 0xfcba5b2d ^ seed;
    uint32_t h4 = 0x32452e3e ^ seed;

    uint32_t c1 = 0x95543787;
    uint32_t c2 = 0x2ad7eb25;

    //----------
    // body

    const uint32_t * blocks = (const uint32_t *)(data);

    for (int i = 0; i < nblocks; i++)
    {
        uint32_t k1 = getblock(blocks, i * 4 + 0);
        uint32_t k2 = getblock(blocks, i * 4 + 1);
        uint32_t k3 = getblock(blocks, i * 4 + 2);
        uint32_t k4 = getblock(blocks, i * 4 + 3);

        bmix32(h1, h2, h3, h4, k1, k2, k3, k4, c1, c2);
    }

    //----------
    // tail

    const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

    uint32_t k1 = 0;
    uint32_t k2 = 0;
    uint32_t k3 = 0;
    uint32_t k4 = 0;

    switch (len & 15)
    {
    case 15: k4 ^= tail[14] << 16;
    case 14: k4 ^= tail[13] << 8;
    case 13: k4 ^= tail[12] << 0;
    case 12: k3 ^= tail[11] << 24;
    case 11: k3 ^= tail[10] << 16;
    case 10: k3 ^= tail[ 9] << 8;
    case  9: k3 ^= tail[ 8] << 0;
    case  8: k2 ^= tail[ 7] << 24;
    case  7: k2 ^= tail[ 6] << 16;
    case  6: k2 ^= tail[ 5] << 8;
    case  5: k2 ^= tail[ 4] << 0;
    case  4: k1 ^= tail[ 3] << 24;
    case  3: k1 ^= tail[ 2] << 16;
    case  2: k1 ^= tail[ 1] << 8;
    case  1: k1 ^= tail[ 0] << 0;
             bmix32(h1, h2, h3, h4, k1, k2, k3, k4, c1, c2);
    };

    //----------
    // finalization

    h4 ^= len;
    h1 += h2;
    h1 += h3;
    h1 += h4;
    h2 += h1;
    h3 += h1;
    h4 += h1;

    h1 = fmix32(h1);
    h2 = fmix32(h2);
    h3 = fmix32(h3);
    h4 = fmix32(h4);

    h1 += h2;
    h1 += h3;
    h1 += h4;
    h2 += h1;
    h3 += h1;
    h4 += h1;

    ((uint32_t*)out)[0] = h1;
    ((uint32_t*)out)[1] = h2;
    ((uint32_t*)out)[2] = h3;
    ((uint32_t*)out)[3] = h4;
}

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

inline uint64_t getblock(const uint64_t * p, int i)
{
    return p[i];
}

//----------
// Block mix - combine the key bits with the hash bits and scramble everything

inline void bmix64(uint64_t & h1, uint64_t & h2, uint64_t & k1, uint64_t & k2,
                   uint64_t & c1, uint64_t & c2)
{
    k1 *= c1;
    k1  = _rotl64(k1, 23);
    k1 *= c2;
    h1 ^= k1;
    h1 += h2;

    h2 = _rotl64(h2, 41);

    k2 *= c2;
    k2  = _rotl64(k2, 23);
    k2 *= c1;
    h2 ^= k2;
    h2 += h1;

    h1 = h1*3+0x52dce729;
    h2 = h2*3+0x38495ab5;

    c1 = c1*5+0x7b7d159c;
    c2 = c2*5+0x6bce6396;
}

//----------
// Finalization mix - avalanches all bits to within 0.05% bias

inline uint64_t fmix64(uint64_t k)
{
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdULL;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53ULL;
    k ^= k >> 33;

    return k;
}

//----------

void MurmurHash3_x64_128(const void * key, const int len, const uint32_t seed, void * out)
{
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 16;

    uint64_t h1 = 0x9368e53c2f6af274ULL ^ seed;
    uint64_t h2 = 0x586dcd208f7cd3fdULL ^ seed;

    uint64_t c1 = 0x87c37b91114253d5ULL;
    uint64_t c2 = 0x4cf5ad432745937fULL;

    //----------
    // body

    const uint64_t * blocks = (const uint64_t *)(data);

    for (int i = 0; i < nblocks; i++)
    {
        uint64_t k1 = getblock(blocks, i * 2 + 0);
        uint64_t k2 = getblock(blocks, i * 2 + 1);

        bmix64(h1, h2, k1, k2, c1, c2);
    }

    //----------
    // tail

    const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch (len & 15)
    {
    case 15: k2 ^= uint64_t(tail[14]) << 48;
    case 14: k2 ^= uint64_t(tail[13]) << 40;
    case 13: k2 ^= uint64_t(tail[12]) << 32;
    case 12: k2 ^= uint64_t(tail[11]) << 24;
    case 11: k2 ^= uint64_t(tail[10]) << 16;
    case 10: k2 ^= uint64_t(tail[ 9]) << 8;
    case  9: k2 ^= uint64_t(tail[ 8]) << 0;

    case  8: k1 ^= uint64_t(tail[ 7]) << 56;
    case  7: k1 ^= uint64_t(tail[ 6]) << 48;
    case  6: k1 ^= uint64_t(tail[ 5]) << 40;
    case  5: k1 ^= uint64_t(tail[ 4]) << 32;
    case  4: k1 ^= uint64_t(tail[ 3]) << 24;
    case  3: k1 ^= uint64_t(tail[ 2]) << 16;
    case  2: k1 ^= uint64_t(tail[ 1]) << 8;
    case  1: k1 ^= uint64_t(tail[ 0]) << 0;
             bmix64(h1, h2, k1, k2, c1, c2);
    };

    //----------
    // finalization

    h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 = fmix64(h1);
    h2 = fmix64(h2);

    h1 += h2;
    h2 += h1;

    ((uint64_t*)out)[0] = h1;
    ((uint64_t*)out)[1] = h2;
}

//-----------------------------------------------------------------------------
// If we need a smaller hash value, it's faster to just use a portion of the
// 128-bit hash

void MurmurHash3_x64_32(const void * key, int len, uint32_t seed, void * out)
{
    uint32_t temp[4];

    MurmurHash3_x64_128(key, len, seed, temp);

    *(uint32_t*)out = temp[0];
}

//----------

void MurmurHash3_x64_64(const void * key, int len, uint32_t seed, void * out)
{
    uint64_t temp[2];

    MurmurHash3_x64_128(key, len, seed, temp);

    *(uint64_t*)out = temp[0];
}

//-----------------------------------------------------------------------------

} // namespace toft
