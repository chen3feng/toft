// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/11/11
// Description: bitmap based byte set class,
//              optimized for both space and performance.

#ifndef COMMON_BASE_STRING_BYTE_SET_H
#define COMMON_BASE_STRING_BYTE_SET_H

#include <ctype.h>
#include <limits.h>
#include <string.h>

// namespace common {

#ifdef _WIN32
#ifndef isblank_defined
#define isblank_defined
inline const bool isblank(int c) {
    return (c == ' ' || c == '   ');
}
#endif
#endif

class ByteSet
{
public:
    ByteSet()
    {
        Clear();
    }

    // fill with pred
    template <typename Pred>
    explicit ByteSet(const Pred& pred)
    {
        Clear();
        InsertPred(pred);
    }

    explicit ByteSet(const char* str)
    {
        Clear();
        InsertString(str);
    }

    explicit ByteSet(const char* str, size_t length)
    {
        Clear();
        InsertBytes(str, length);
    }

    // insert one byte
    void Insert(unsigned char n)
    {
        m_bitmap[n/CHAR_BIT] |= (1U << (n%CHAR_BIT));
    }

    // remove one byte
    void Remove(unsigned char n)
    {
        m_bitmap[n/CHAR_BIT] &= ~(1U << (n%CHAR_BIT));
    }

    // clear all bytes
    void Clear()
    {
        memset(m_bitmap, 0, sizeof(m_bitmap));
    }

    // insert bytes by pred
    // bool pred(unsigned char byte)
    template <typename Pred>
    void InsertPred(const Pred& pred)
    {
        for (int c = 0; c <= UCHAR_MAX; ++c)
        {
            if (pred(c))
                Insert(c);
        }
    }

    // insert any bytes in str into set
    void InsertString(const char* str)
    {
        while (*str)
        {
            Insert(*str);
            ++str;
        }
    }

    // insert any bytes in str into set
    void InsertBytes(const char* begin, size_t length)
    {
        for (size_t i = 0; i < length; ++i)
        {
            Insert(begin[i]);
        }
    }

    // remove all bytes which satisfy the pred
    template <typename Pred>
    void RemovePred(const Pred& pred)
    {
        for (int c = 0; c <= UCHAR_MAX; ++c)
        {
            if (pred(c))
                Remove(c);
        }
    }

    // remove all bytes in str from the set
    void Remove(const char* str)
    {
        while (*str)
        {
            Remove(*str);
            ++str;
        }
    }

    // return whether exist
    bool Find(unsigned char c) const
    {
        return (m_bitmap[c/CHAR_BIT] >> (c%CHAR_BIT)) & 1;
    }

    // overload this operator make it can be used as function object to adapt
    // STL algorithms
    bool operator()(unsigned char c) const
    {
        return Find(c);
    }

    // OR the set with rhs
    ByteSet& operator|=(const ByteSet& rhs)
    {
        for (size_t i = 0; i < sizeof(m_bitmap)/sizeof(m_bitmap[0]); ++i)
            m_bitmap[i] |= rhs.m_bitmap[i];
        return *this;
    }

    // AND the set with rhs
    ByteSet& operator&=(const ByteSet& rhs)
    {
        for (size_t i = 0; i < sizeof(m_bitmap)/sizeof(m_bitmap[0]); ++i)
            m_bitmap[i] &= rhs.m_bitmap[i];
        return *this;
    }

    // OR the set with string
    ByteSet& operator|=(const char* rhs)
    {
        InsertString(rhs);
        return *this;
    }

public: // predefined singletons
    // return the set of all bytes satisfy isspace
    static const ByteSet& SpaceSet()
    {
        static const ByteSet cs(isspace);
        return cs;
    }

    // return the set of all bytes satisfy isblank
    static const ByteSet& BlankSet()
    {
        static const ByteSet cs(isblank);
        return cs;
    }

    // return the set of all bytes satisfy isspace
    static const ByteSet& AlphaSet()
    {
        static const ByteSet cs(isalpha);
        return cs;
    }

    // return the set of all bytes satisfy isalnum
    static const ByteSet& AlphaNumSet()
    {
        static const ByteSet cs(isalnum);
        return cs;
    }

    // return the set of all bytes satisfy isascii
    static const ByteSet& AsciiSet()
    {
        static const ByteSet cs(isascii);
        return cs;
    }

    // return the set of all bytes satisfy isxdigit
    static const ByteSet& HexSet()
    {
        static const ByteSet cs(isxdigit);
        return cs;
    }

    // return the set of all bytes satisfy isdigit
    static const ByteSet& DigitSet()
    {
        static const ByteSet cs(isdigit);
        return cs;
    }

    // return the set of all bytes satisfy isupper
    static const ByteSet& UpperSet()
    {
        static const ByteSet cs(isupper);
        return cs;
    }

    // return the set of all bytes satisfy islower
    static const ByteSet& LowerSet()
    {
        static const ByteSet cs(islower);
        return cs;
    }

    // return the set of all bytes satisfy isprint
    static const ByteSet& PrintSet()
    {
        static const ByteSet cs(isprint);
        return cs;
    }

private:
    // using 256 bits to save byte set
    unsigned char m_bitmap[(UCHAR_MAX + 1 + CHAR_BIT - 1) / CHAR_BIT];
};

///////////////////////////////////////////////////////////////////////////////
// global operators

inline const ByteSet operator|(const ByteSet& lhs, const ByteSet& rhs)
{
    return ByteSet(lhs) |= rhs;
}

inline const ByteSet operator|(const ByteSet& lhs, const char* rhs)
{
    ByteSet r(lhs);
    return  r |= rhs;
}

inline const ByteSet operator&(const ByteSet& lhs, const ByteSet& rhs)
{
    ByteSet r(lhs);
    return r &= rhs;
}

// } // namespace common

#endif // COMMON_BASE_STRING_BYTE_SET_H
