// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-07

#ifndef TOFT_BASE_STRING_FORMAT_SPECIFICATION_H
#define TOFT_BASE_STRING_FORMAT_SPECIFICATION_H
#pragma once

#include <limits.h>
#include <stddef.h>
#include <string.h>

namespace toft {

const int kFormatLengthHH = ('h' << CHAR_BIT) + 'h'; // 'hh'
const int kFormatLengthLL = ('l' << CHAR_BIT) + 'l'; // 'll'

struct FormatSpecification {
    int width;          // field width
    int length;         // length modifier, such as h, hh, l, ll
    char specifier;     // c, d, x, o, i, s, p...

    FormatSpecification() :
        width(-1),
        length(0),
        specifier('\0')
    {
    }

    bool has_width() const { return width != -1; }
    bool has_length() const { return length != 0; }

    bool SpecifierMatch(char s1) const
    {
        return specifier == 'v' || specifier == s1;
    }

    bool SpecifierMatch(const char* ss) const
    {
        return specifier == 'v' || strchr(ss, specifier) != NULL;
    }

    bool LengthMatch(int allowed) const
    {
        return length == 0 || length == allowed;
    }

    template <size_t N>
    bool LengthMatch(const int (&allowed)[N]) const
    {
        if (length == 0)
            return true;
        for (size_t i = 0; i < N - 1; ++i) {
            if (allowed[i] == length)
                return true;
        }
        return false;
    }

    const char* LengthToString() const;
};

struct ScanSpecification : FormatSpecification {
    bool skip;          // starts with '*' means to skip this field
    char scanset[32];   // Used only if specifier == '['

    ScanSpecification() : skip(false) { }

    // Return number of chars consumed.
    int Parse(const char* format);
};

struct PrintSpecification : FormatSpecification {
    struct Flags {
        bool left;
        bool sign;
        bool space;
        bool sharp;
        bool zero;
    } flags;
    int precision;      // field precision, -1 means not set, -'*' means '*'

    PrintSpecification() :
        flags(),
        precision(-1)
    {
    }
    // Return number of chars consumed.
    int Parse(const char* format);

    bool has_precision() const { return precision != -1; }
};

} // namespace toft

#endif // TOFT_BASE_STRING_FORMAT_SPECIFICATION_H
