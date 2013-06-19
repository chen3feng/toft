// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-07

#include "toft/base/string/format/specification.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "thirdparty/glog/logging.h"

namespace toft {

const char* FormatSpecification::LengthToString() const
{
    switch (length) {
    case 0: return "";
    case 'h': return "h";
    case 'l': return "l";
    case 'L': return "L";
    case 'j': return "j";
    case 'z': return "t";
    case 't': return "t";
    case kFormatLengthHH:
        return "hh";
    case kFormatLengthLL:
        return "ll";
    }

    DCHECK(false) << "Invalid length: " << static_cast<char>(length);
    return NULL;
}

static int ParseWidth(const char* format, int* width)
{
    if (!isdigit(*format))
        return 0;
    char* endptr;
    *width = strtoul(format, &endptr, 10);
    if (*width < 0)
        return 0;
    return endptr - format;;
}

static int ParseLength(const char* format, int* length)
{
    const char* f = format;
    switch (*f) {
    case 'h':
    case 'l':
        if (f[1] == f[0]) {
            *length = (f[1] << 8) + f[0];
            f += 2;
        } else {
            *length = f[0];
            ++f;
        }
        break;
    case 'j':
    case 'z':
    case 't':
    case 'L':
        *length = *f;
        ++f;
        break;
    default:
        *length = 0;
    }

    return f - format;
}

static inline void ScanSetSetChar(char (*cs)[32], char c, bool value)
{
    int index = static_cast<unsigned char>(c);
    int offset = index / CHAR_BIT;
    int mask = (1 << (index % CHAR_BIT));
    if (value)
        (*cs)[offset] |= mask;
    else
        (*cs)[offset] &= ~mask;
}

static void ScanSetSetCharRange(char (*cs)[32], char first, char last, bool value)
{
    for (char c = first; c <= last; ++c)
        ScanSetSetChar(cs, c, value);
}

static int ParseScanSet(const char* format, char (*cs)[32])
{
    const char* f = format;
    bool is_neg = *f == '^';
    if (is_neg) {
        memset(cs, 0xFF, sizeof(*cs));
        ++f;
    } else {
        memset(cs, 0, sizeof(*cs));
    }

    while (*f != ']') {
        if (f - format > 256) {
            LOG(DFATAL) << "Invalid scanset, too long to see the ending ']'";
            return 0;
        }
        if (*f == '\0') {
            LOG(DFATAL) << "Invalid scanset, unexpected end before ']'";
            return 0;
        }

        if (f[1] == '-' && f[2] != '\0' && f[2] != ']') {
            // Process char range
            ScanSetSetCharRange(cs, f[0], f[2], !is_neg);
            f += 3;
        } else {
            // Single char
            ScanSetSetChar(cs, *f, !is_neg);
            f += 1;
        }
    }
    ++f;

    return f - format;
}

static int ParseSpecifier(const char* format, char* specifier)
{
    const char* f = format;
    switch (*f) {
    case 'A': // float
    case 'E': // float
    case 'F': // float
    case 'G': // float
    case 'X': // hexadecimal int
    case 'a': // float
    case 'b': // bool
    case 'c': // char
    case 'd': // decimal int
    case 'e': // float
    case 'f': // float
    case 'g': // float
    case 'i': // int
    case 'n': // output size scanned
    case 'o': // octal int
    case 'p': // pointer
    case 's': // string
    case 'u': // unsigned int
    case 'v': // Automatic type
    case 'x': // hexadecimal int
        *specifier = *f;
        ++f;
        break;
    }
    return f - format;
}

static int ParseScanSpecifier(const char* format, ScanSpecification* spec)
{
    const char* f = format;
    int n = ParseSpecifier(f, &spec->specifier);
    if (n > 0)
        return n;

    if (*f == '[') {
        int width = ParseScanSet(f + 1, &spec->scanset);
        if (width > 0) {
            spec->specifier = '[';
            f += width + 1;
        }
    } else {
        LOG(DFATAL) << "Invalid scan specifier: " << f[0];
    }

    return f - format;
}

static int ParsePrintSpecifier(const char* format, char* specifier)
{
    int n = ParseSpecifier(format, specifier);
    if (n == 0) {
        LOG(DFATAL) << "Invalid print specifier: " << format[0];
    }
    return n;
}

int ScanSpecification::Parse(const char* format)
{
    const char* f = format;
    if (*f == '*') {
        ++f;
        this->skip = true;
    }

    f += ParseWidth(f, &this->width);
    f += ParseLength(f, &this->length);

    int this_width = ParseScanSpecifier(f, this);
    if (this_width == 0)
        return 0;
    f += this_width;

    return f - format;
}

static int ParseFlags(const char* format, PrintSpecification::Flags* flags)
{
    const char* f = format;
    while (*f != '\0') {
        switch (*f) {
        case '-':
            flags->left = true;
            break;
        case '+':
            flags->sign = true;
            break;
        case ' ':
            flags->space = true;
            break;
        case '#':
            flags->sharp = true;
            break;
        case '0':
            flags->zero = true;
            break;
        default:
            return f - format;
        }
        ++f;
    }
    return f - format;
}

// The width and precision of print allow '*'
static int ParsePrintWidth(const char* format, int* value)
{
    const char* f = format;
    int n = ParseWidth(f, value);
    if (n > 0) {
        f += n;
    } else if (*f == '*') {
        *value = -'*';
        ++f;
    }
    return f - format;
}

int PrintSpecification::Parse(const char* format)
{
    const char* f = format;

    f += ParseFlags(f, &this->flags);
    f += ParsePrintWidth(f, &this->width);

    if (*f == '.') {
        ++f;
        int n = ParsePrintWidth(f, &this->precision);
        if (n > 0)
            f += n;
        else
            return 0;
    }

    f += ParseLength(f, &this->length);
    int n = ParsePrintSpecifier(f, &this->specifier);
    if (n == 0)
        return 0;
    f += n;

    return f - format;
}

} // namespace toft
