// Copyright (c) 2011, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

// GLOBAL_NOLINT(readability/casting)

#ifndef TOFT_BASE_STRING_COMPARE_H
#define TOFT_BASE_STRING_COMPARE_H
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string>

#include "toft/base/static_assert.h"
#include "toft/base/string/string_piece.h"
#include "toft/system/memory/unaligned.h"

namespace toft {

namespace internals {

inline bool memeql_2(const char* p1, const char* p2)
{
    return *(uint16_t*)&p1[0] == *(uint16_t*)&p2[0];
}

inline bool memeql_3(const char* p1, const char* p2)
{
    return
        *(uint16_t*)&p1[0] == *(uint16_t*)&p2[0] &&
        p1[2] == p2[2];
}

inline bool memeql_4(const char* p1, const char* p2)
{
    return
        *(uint32_t*)&p1[0] == *(uint32_t*)&p2[0];
}

inline bool memeql_5(const char* p1, const char* p2)
{
    return
        *(uint32_t*)&p1[0] == *(uint32_t*)&p2[0] &&
        p1[4] == p2[4];
}

inline bool memeql_6(const char* p1, const char* p2)
{
    return
        *(uint32_t*)&p1[0] == *(uint32_t*)&p2[0] &&
        *(uint16_t*)&p1[4] == *(uint16_t*)&p2[4];
}

inline bool memeql_7(const char* p1, const char* p2)
{
    return
        *(uint32_t*)&p1[0] == *(uint32_t*)&p2[0] &&
        *(uint16_t*)&p1[4] == *(uint16_t*)&p2[4] &&
        p1[6] == p2[6];
}

inline bool memeql_8(const char* p1, const char* p2)
{
    return *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0];
}

inline bool memeql_9(const char* p1, const char* p2)
{
    return
        *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
        p1[8] == p2[8];
}

inline bool memeql_10(const char* p1, const char* p2)
{
    return
        *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
        *(uint16_t*)&p1[8] == *(uint16_t*)&p2[8];
}

inline bool memeql_11(const char* p1, const char* p2)
{
    return
        *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
        *(uint16_t*)&p1[8] == *(uint16_t*)&p2[8] &&
        p1[10] == p2[10];
}

inline bool memeql_12(const char* p1, const char* p2)
{
    return
        *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
        *(uint32_t*)&p1[8] == *(uint32_t*)&p2[8];
}

inline bool memeql_13(const char* p1, const char* p2)
{
    return
        *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
        *(uint32_t*)&p1[8] == *(uint32_t*)&p2[8] &&
        p1[12] == p2[12];
}

inline bool memeql_14(const char* p1, const char* p2)
{
    return
        *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
        *(uint32_t*)&p1[8] == *(uint32_t*)&p2[8] &&
        *(uint16_t*)&p1[12] == *(uint16_t*)&p2[12];
}

inline bool memeql_15(const char* p1, const char* p2)
{
    return
        *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
        *(uint32_t*)&p1[8] == *(uint32_t*)&p2[8] &&
        *(uint16_t*)&p1[12] == *(uint16_t*)&p2[12] &&
        p1[14] == p2[14];
}

inline bool memeql_16(const char* p1, const char* p2)
{
    return
        *(uint64_t*)&p1[0] == *(uint64_t*)&p2[0] &&
        *(uint64_t*)&p1[8] == *(uint64_t*)&p2[8];
}

} // namespace internals

// An optimized fast memory compare function, should be inlined
inline bool memeql(const void* a1, const void* a2, size_t size)
{
#if TOFT_ALIGNMENT_INSENSITIVE_PLATFORM
    using namespace internals; // NOLINT(build/namespaces)
    // optimize for alignment insensitive architectures
    const char* p1 = (const char*)a1;
    const char* p2 = (const char*)a2;

    switch (size)
    {
    case 0:
        return true;
    case 1:
        return p1[0] == p2[0];
    case 2:
        return memeql_2(p1, p2);
    case 3:
        return memeql_3(p1, p2);
    case 4:
        return memeql_4(p1, p2);
    case 5:
        return memeql_5(p1, p2);
    case 6:
        return memeql_6(p1, p2);
    case 7:
        return memeql_7(p1, p2);
    case 8:
        return memeql_8(p1, p2);
    case 9:
        return memeql_9(p1, p2);
    case 10:
        return memeql_10(p1, p2);
    case 11:
        return memeql_11(p1, p2);
    case 12:
        return memeql_12(p1, p2);
    case 13:
        return memeql_13(p1, p2);
    case 14:
        return memeql_14(p1, p2);
    case 15:
        return memeql_15(p1, p2);
    case 16:
        return memeql_16(p1, p2);
    }

    while (size >= 8)
    {
        if (*(uint64_t*)&p1[0] != *(uint64_t*)&p2[0])
            return false;
        p1 += 8;
        p2 += 8;
        size -= 8;
    }
    if (size >= 4)
    {
        if (*(uint32_t*)&p1[0] != *(uint32_t*)&p2[0])
            return false;
        p1 += 4;
        p2 += 4;
        size -= 4;
    }
    if (size >= 2)
    {
        if (*(uint16_t*)&p1[0] != *(uint16_t*)&p2[0])
            return false;
        p1 += 2;
        p2 += 2;
        size -= 2;
    }
    if (size == 1)
        return p1[0] == p2[0];

    return true;
#else
    return memcmp(a1, a2, size) == 0;
#endif
}

int memcasecmp(const void *vs1, const void *vs2, size_t n);

// An optimized fast memory compare function, should be inlined
inline bool MemoryEqual(const void* a1, const void* a2, size_t size)
{
    return memeql(a1, a2, size);
}

inline int CompareMemory(const void *b1, const void *b2, size_t len, size_t* prefix_length)
{
    TOFT_STATIC_ASSERT(sizeof(size_t) == 8 || sizeof(size_t) == 4 || sizeof(size_t) == 2);

    const unsigned char * const a = (const unsigned char *)b1;
    const unsigned char * const b = (const unsigned char *)b2;

    // pos must bu signed type
    ptrdiff_t pos = 0;
    ptrdiff_t end_pos = len - sizeof(size_t);

    int result = 0;

#define COMPARE_MEMORY_ONE_BYTE() \
    result = a[pos] - b[pos]; \
    if (result) { \
        *prefix_length = pos;\
        return result;\
    } \
    ++pos

    while (pos <= end_pos) // compare by word size
    {
        if (GetUnaligned<size_t>(a + pos) != GetUnaligned<size_t>(b + pos))
        {
            switch (sizeof(size_t))
            {
            case 8:
                COMPARE_MEMORY_ONE_BYTE();
                COMPARE_MEMORY_ONE_BYTE();
                COMPARE_MEMORY_ONE_BYTE();
                COMPARE_MEMORY_ONE_BYTE();
                // fall through
            case 4:
                COMPARE_MEMORY_ONE_BYTE();
                COMPARE_MEMORY_ONE_BYTE();
                // fall through
            case 2:
                COMPARE_MEMORY_ONE_BYTE();
                COMPARE_MEMORY_ONE_BYTE();
            }
            assert(!"unreachable");
        }
        pos += sizeof(size_t);
    }

    switch (len - pos) // handle tail
    {
    case 7: COMPARE_MEMORY_ONE_BYTE();
    case 6: COMPARE_MEMORY_ONE_BYTE();
    case 5: COMPARE_MEMORY_ONE_BYTE();
    case 4: COMPARE_MEMORY_ONE_BYTE();
    case 3: COMPARE_MEMORY_ONE_BYTE();
    case 2: COMPARE_MEMORY_ONE_BYTE();
    case 1: COMPARE_MEMORY_ONE_BYTE();
    }

#undef COMPARE_MEMORY_ONE_BYTE

    *prefix_length = len;
    return result; // match
}

inline int CompareMemory(const void *b1, const void *b2, size_t len)
{
    size_t prefix_length;
    return CompareMemory(b1, b2, len, &prefix_length);
}

inline size_t GetCommonPrefixLength(
    const void* lhs, size_t lhs_len,
    const void* rhs, size_t rhs_len
)
{
    size_t prefix_length;
    size_t common_length = lhs_len < rhs_len ? lhs_len : rhs_len;
    CompareMemory(lhs, rhs, common_length, &prefix_length);
    return prefix_length;
}

inline size_t GetCommonPrefixLength(const std::string& lhs, const std::string& rhs)
{
    return GetCommonPrefixLength(lhs.c_str(), lhs.length(),
            rhs.c_str(), rhs.length());
}

inline int CompareByteString(const void* lhs, size_t lhs_len,
        const void* rhs, size_t rhs_len, bool* inclusive,
        size_t* common_prefix_len = NULL)
{
    const unsigned char* p1 = reinterpret_cast<const unsigned char*>(lhs);
    const unsigned char* p2 = reinterpret_cast<const unsigned char*>(rhs);
    ptrdiff_t min_len = (lhs_len <= rhs_len) ? lhs_len : rhs_len;
    ptrdiff_t pos = 0;
    ptrdiff_t end_pos = min_len - sizeof(size_t) + 1;

    while (pos < end_pos)
    {
        if (GetUnaligned<size_t>(p1 + pos) == GetUnaligned<size_t>(p2 + pos))
            pos += sizeof(size_t);
        else
            break;
    }

    while ((pos < min_len) && (p1[pos] == p2[pos]))
        pos++;

    *inclusive = (pos == min_len);

    if (common_prefix_len != NULL)
        *common_prefix_len = pos;

    if (*inclusive)
    {
        if (lhs_len > rhs_len)
            return 1;
        else if (lhs_len == rhs_len)
            return 0;
        else
            return -1;
    }
    else
    {
        return p1[pos] - p2[pos];
    }
}

inline int CompareByteString(
    const void* lhs, size_t lhs_len,
    const void* rhs, size_t rhs_len
)
{
    bool inclusive;
    return CompareByteString(lhs, lhs_len, rhs, rhs_len, &inclusive);
}

inline int CompareByteString(const std::string& lhs, const std::string& rhs)
{
    return CompareByteString(lhs.c_str(), lhs.length(), rhs.c_str(), rhs.length());
}

inline int CompareStringIgnoreCase(const StringPiece& lhs, const StringPiece& rhs)
{
    return lhs.ignore_case_compare(rhs);
}

inline bool StringEqualsIgnoreCase(const StringPiece& lhs, const StringPiece& rhs)
{
    return lhs.ignore_case_equal(rhs);
}

} // namespace toft

#endif // TOFT_BASE_STRING_COMPARE_H
