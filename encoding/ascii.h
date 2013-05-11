// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_ENCODING_ASCII_H
#define TOFT_ENCODING_ASCII_H

#include <limits.h>
#include <stdint.h>

namespace toft {

// Used for ASCII encoding only.
// If you want to process locale ralated text, please using <ctype.h>
struct Ascii
{
private:
    Ascii();
    ~Ascii();
private:
    enum CharTypeMask
    {
        kUpper = 1 << 0,
        kLower = 1 << 1,
        kDigit = 1 << 2,
        kHexDigit = 1 << 3,
        kBlank = 1 << 4,
        kSpace = 1 << 5,
        kControl = 1 << 6,
        kPunct = 1 << 7,
        kPrint = 1 << 8,
        kGraph = 1 << 9,
    };
public:
    static bool IsValid(char c)
    {
        return (c & 0x80) == 0;
    }

    static inline bool IsLower(char c)
    {
        return CharIncludeAnyTypeMask(c, kLower);
    }

    static inline bool IsUpper(char c)
    {
        return CharIncludeAnyTypeMask(c, kUpper);
    }

    static bool IsAlpha(char c)
    {
        return CharIncludeAnyTypeMask(c, kUpper | kLower);
    }

    static bool IsDigit(char c)
    {
        return CharIncludeAnyTypeMask(c, kDigit);
    }

    static bool IsAlphaNumber(char c)
    {
        return CharIncludeAnyTypeMask(c, kUpper | kLower | kDigit);
    }

    static bool IsBlank(char c)
    {
        return CharIncludeAnyTypeMask(c, kBlank);
    }

    static inline bool IsSpace(char c)
    {
        return CharIncludeAnyTypeMask(c, kSpace);
    }

    static bool IsControl(char c)
    {
        return CharIncludeAnyTypeMask(c, kControl);
    }

    static inline bool IsPunct(char c)
    {
        return CharIncludeAnyTypeMask(c, kPunct);
    }

    static inline bool IsHexDigit(char c)
    {
        return CharIncludeAnyTypeMask(c, kHexDigit);
    }

    static inline bool IsGraph(char c)
    {
        return CharIncludeAnyTypeMask(c, kGraph);
    }

    static inline bool IsPrint(char c)
    {
        return CharIncludeAnyTypeMask(c, kPrint);
    }

    static inline char ToAscii(char c)
    {
        return c & 0x7F;
    }

    static inline char ToLower(char c)
    {
        return IsUpper(c) ? c + ('a' - 'A') : c;
    }

    static inline char ToUpper(char c)
    {
        return IsLower(c) ? c - ('a' - 'A') : c;
    }

private:
    static int GetCharTypeMask(char c)
    {
        return table[static_cast<unsigned char>(c)];
    }

    static bool CharIncludeAnyTypeMask(char c, int mask)
    {
        return (GetCharTypeMask(c) & mask) != 0;
    }

    static bool CharIncludeAallTypeMask(char c, int mask)
    {
        return (GetCharTypeMask(c) & mask) == mask;
    }

private:
    static const uint16_t table[UCHAR_MAX + 1];
};

} // namespace toft

#endif // TOFT_ENCODING_ASCII_H
