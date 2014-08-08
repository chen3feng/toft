// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-05

#include "toft/base/string/format/scan.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "toft/base/string/number.h"

#include "thirdparty/glog/logging.h"

// GLOBAL_NOLINT(runtime/int)
// GLOBAL_NOLINT(readability/function)

namespace toft {

static inline bool ScanSetContainsChar(const char (&cs)[32], char c)
{
    int index = static_cast<unsigned char>(c);
    int offset = index / CHAR_BIT;
    int mask = (1 << (index % CHAR_BIT));
    return (cs[offset] & mask) != 0;
}

static void SkipLeadingSpaces(const char** str)
{
    const char* s = *str;
    while (*s != '\0' && isspace(*s))
        ++s;
    *str = s;
}

int ParseVoidPtr(const char* str, const ScanSpecification& spec, void** value)
{
    if (!spec.SpecifierMatch('p')) {
        LOG(DFATAL) << "Invalid scan specifier '" << spec.specifier
                    << "' for pointer type";
        return 0;
    }
    char* endptr;
    unsigned long n = strtoul(str, &endptr, 16);
    *value = reinterpret_cast<void*>(n);
    return endptr - str;
}

int ScanValueParser<bool>::Parse(const char* str, const ScanSpecification& spec, bool* value)
{
    if (!spec.SpecifierMatch('b')) {
        LOG(DFATAL) << "Invalid scan specifier '" << spec.specifier
                    << "' for bool type";
        return 0;
    }
    const char* s = str;
    SkipLeadingSpaces(&s);
    if (*s == '1') {
        *value = true;
        ++s;
    } else if (*s == '0') {
        *value = false;
        ++s;
    } else if (strncmp(s, "true", 4) == 0) {
        *value = true;
        s += 4;
    } else if (strncmp(s, "false", 5) == 0) {
        *value = false;
        s += 5;
    } else {
        return 0;
    }
    return s - str;
}

int ScanValueParser<char>::Parse(const char* str, const ScanSpecification& spec, char* value)
{
    if (!spec.SpecifierMatch('c')) {
        LOG(DFATAL) << "Invalid scan specifier '" << spec.specifier
                    << "' for char type";
        return 0;
    }
    *value = *str;
    return 1;
}

template <typename Type>
static int ParseSigned(const char* str, const ScanSpecification& spec, Type* value)
{
    int base = 0;
    switch (spec.specifier) {
    case 'v':
    case 'i':
        break;
    case 'd':
        base = 10;
        break;
    case 'x':
    case 'X':
        base = 16;
        break;
    case 'o':
        base = 8;
        break;
    default:
        LOG(DFATAL) << "Invalid scan specifier '" << spec.specifier
                    << "' for signed integral type";
        return 0;
    }

    char* endptr;
    int saved_errno = errno;
    Type v;
    errno = 0;
    if (sizeof(Type) > sizeof(long)) {
        long long n = strtoll(str, &endptr, base);
        if (errno != 0)
            return 0;
        v = n;
    } else {
        long n = strtol(str, &endptr, base);
        if (errno != 0)
            return 0;
        v = n;
        if (v != n) {
            errno = ERANGE;
            return false;
        }
    }

    *value = v;
    errno = saved_errno;
    return endptr - str;
}

template <typename Type>
static int ParseUnsigned(const char* str, const ScanSpecification& spec, Type* value)
{
    int base = 0;
    switch (spec.specifier) {
    case 'v':
    case 'i':
        break;
    case 'd':
    case 'u':
        base = 10;
        break;
    case 'x':
    case 'X':
        base = 16;
        break;
    case 'o':
        base = 8;
        break;
    default:
        LOG(DFATAL) << "Invalid scan specifier '" << spec.specifier
                    << "' for unsigned integral type";
        return 0;
    }

    const char* nonspace = str;
    while (*nonspace && isspace(*nonspace)) { // Skip leading spaces
        ++nonspace;
    }
    if (*nonspace == '-') {
        errno = ERANGE;
        return 0;
    }

    char* endptr;
    int saved_errno = errno;
    Type v;
    errno = 0;
    if (sizeof(Type) > sizeof(long)) {
        unsigned long long n = strtoull(nonspace, &endptr, base);
        if (errno != 0)
            return 0;
        v = n;
    } else {
        unsigned long n = strtoul(nonspace, &endptr, base);
        if (errno != 0)
            return 0;
        v = n;
        if (v != n) {
            errno = ERANGE;
            return 0;
        }
    }

    *value = v;
    errno = saved_errno;
    return endptr - str;
}

#define STRING_SCAN_DEFINE_SIGNED_INT_PARSE(Type) \
int ScanValueParser<Type>::Parse(const char* str, \
                                 const ScanSpecification& spec, \
                                 Type* value) \
{ \
    return ParseSigned(str, spec, value); \
}

#define STRING_SCAN_DEFINE_UNSIGNED_INT_PARSE(Type) \
int ScanValueParser<Type>::Parse(const char* str, \
                                 const ScanSpecification& spec, \
                                 Type* value) \
{ \
    return ParseUnsigned(str, spec, value); \
}

STRING_SCAN_DEFINE_SIGNED_INT_PARSE(signed char)
STRING_SCAN_DEFINE_SIGNED_INT_PARSE(short)
STRING_SCAN_DEFINE_SIGNED_INT_PARSE(int)
STRING_SCAN_DEFINE_SIGNED_INT_PARSE(long)
STRING_SCAN_DEFINE_SIGNED_INT_PARSE(long long)

STRING_SCAN_DEFINE_UNSIGNED_INT_PARSE(unsigned char)
STRING_SCAN_DEFINE_UNSIGNED_INT_PARSE(unsigned short)
STRING_SCAN_DEFINE_UNSIGNED_INT_PARSE(unsigned int)
STRING_SCAN_DEFINE_UNSIGNED_INT_PARSE(unsigned long)
STRING_SCAN_DEFINE_UNSIGNED_INT_PARSE(unsigned long long)

#undef STRING_SCAN_DEFINE_SIGNED_INT_PARSE
#undef STRING_SCAN_DEFINE_UNSIGNED_INT_PARSE

template <typename Type>
static int ParseFloat(const char* str, const ScanSpecification& spec, Type* value)
{
    if (!spec.SpecifierMatch("aefgAEFG")) {
        LOG(DFATAL) << "Invalid scan specifier '" << spec.specifier
                    << "' for float type";
        return 0;
    }

    const char* s = str;
    SkipLeadingSpaces(&s);
    char* endptr;
    Type v;

    if (sizeof(Type) == sizeof(float)) { // NOLINT(runtime/sizeof)
        v = strtof(s, &endptr);
    } else if (sizeof(Type) == sizeof(double)) { // NOLINT(runtime/sizeof)
        v = strtod(s, &endptr);
    } else if (sizeof(Type) == sizeof(long double)) { // NOLINT(runtime/sizeof)
        v = strtold(s, &endptr);
    }

    if (endptr == s) // Nothing parsed
        return 0;

    *value = v;
    return endptr - str;
}

int ScanValueParser<float>::Parse(const char* str,
                                  const ScanSpecification& spec,
                                  float* value)
{
    if (spec.length != 0) {
        LOG(DFATAL) << "Invalid scan specifier '" << spec.LengthToString()
                    << "' for float type";
        return false;
    }
    return ParseFloat(str, spec, value);
}

int ScanValueParser<double>::Parse(const char* str,
                                   const ScanSpecification& spec,
                                   double* value)
{
    if (spec.length != 0 && spec.length != 'l') {
        LOG(DFATAL) << "Invalid scan specifier '" << spec.LengthToString()
                    << "' for double type";
        return false;
    }
    return ParseFloat(str, spec, value);
}

int ScanValueParser<long double>::Parse(const char* str,
                                        const ScanSpecification& spec,
                                        long double* value)
{
    if (spec.length != 0 && spec.length != 'L') {
        LOG(DFATAL) << "Invalid scan specifier '" << spec.LengthToString()
                    << "' for long double type";
        return false;
    }
    return ParseFloat(str, spec, value);
}

int ScanValueParser<std::string>::Parse(const char* str,
                                        const ScanSpecification& spec,
                                        std::string* value)
{
    const char* first = str;
    const char* last = first;
    if (spec.specifier == 's' || spec.specifier == 'v') {
        SkipLeadingSpaces(&first);
        last = first;
        while (*last != '\0' && !isspace(*last)) {
            ++last;
        }
    } else if (spec.specifier == '[') {
        while (*last != '\0' && ScanSetContainsChar(spec.scanset, *last)) {
            ++last;
        }
    } else {
        LOG(DFATAL) << "Invalid scan specifier '" << spec.specifier
                    << "' for string type";
        return 0;
    }

    if (first == last) {
        LOG(ERROR) << "Invalid string field: empty";
        return 0;
    }

    value->assign(first, last);
    return last - str;
}

int FormatScanArg::ParseSkipped(const char* string, const ScanSpecification& spec)
{
#define CASE_TYPE(Type) \
    do { \
        Type a; \
        FormatScanArg sa(&a); \
        return sa.Parse(string, spec); \
    } while (0)

    switch (spec.specifier) {
    case 'c':
        CASE_TYPE(char);
    case 'd':
    case 'i':
        switch (spec.length) {
        case 'h':
            CASE_TYPE(short);
        case 'l':
            CASE_TYPE(long);
        case kFormatLengthHH:
            CASE_TYPE(signed char);
        case kFormatLengthLL: // Fallthrough
        default:
            CASE_TYPE(long long);
        }
        break;
    case 'u':
    case 'o':
    case 'x':
        switch (spec.length) {
        case 'h':
            CASE_TYPE(unsigned short);
        case 'l':
            CASE_TYPE(unsigned long);
        case kFormatLengthHH:
            CASE_TYPE(unsigned char);
        case kFormatLengthLL: // Fallthrough
        default:
            CASE_TYPE(unsigned long long);
        }
        break;
    case 'A':
    case 'E':
    case 'F':
    case 'G':
    case 'a':
    case 'e':
    case 'f':
    case 'g':
        switch (spec.length) {
        case 'l':
            CASE_TYPE(double);
        case kFormatLengthLL: // Fallthrough
        case 'L':
        default:
            CASE_TYPE(long double);
        }
        break;
    case 'p':
        CASE_TYPE(void*);
    case 's':
    case '[':
        CASE_TYPE(std::string);
    case 'n':
        break;
    case 'v':
        LOG(DFATAL) << "Invalid scan specifier 'v' for ignored fields";
    }

    LOG(DFATAL) << "Invalid scan specifier for ignored fields: " << spec.specifier;
    return 0;
#undef CASE_TYPE
}

int FormatScanArg::Parse(const char* buf, const ScanSpecification& spec) const
{
    return m_parse(buf, spec, m_ptr);
}

// This function is rarely used, so don't add another function pointer to
// reduce performance of most common case.
bool FormatScanArg::WriteInt(int n, const ScanSpecification& spec) const
{
    char buf[16];
    IntegerToString(n, buf);

    ScanSpecification new_spec(spec);
    new_spec.specifier = 'd';
    return Parse(buf, new_spec) > 0;
}

} // namespace toft
