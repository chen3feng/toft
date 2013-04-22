// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-07

#include "toft/base/string/format/print_arg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "toft/base/string/format/print_targets.h"
#include "toft/base/string/number.h"

#include "thirdparty/glog/logging.h"

// GLOBAL_NOLINT(runtime/int)

namespace toft {

static int WriteString(FormatPrintTarget* target,
                       const PrintSpecification& spec,
                       const char* text, int size)
{
    if (!spec.has_width() || size >= spec.width) {
        target->WriteString(text, size);
        return size;
    }

    int padding = spec.width - size;
    if (spec.flags.left) {
        target->WriteString(text, size);
        target->WriteChars(' ', padding);
    } else {
        target->WriteChars(' ', padding);
        target->WriteString(text, size);
    }
    return spec.width;
}

static int WriteNumberString(FormatPrintTarget* target,
                             const PrintSpecification& spec,
                             char sign,
                             const char* prefix, int prefix_length,
                             int leading_zeros,
                             const char* digits, int digits_length,
                             int padding_zeros,
                             const char* exp, int exp_length
)
{
    int num_size = (sign != '\0') + prefix_length + leading_zeros +
                   digits_length + padding_zeros + exp_length;

    if (!spec.has_width() || spec.width <= num_size) {
        if (sign)
            target->WriteChar(sign);
        target->WriteString(prefix, prefix_length);
        target->WriteChars('0', leading_zeros);
        target->WriteString(digits, digits_length);
        target->WriteChars('0', padding_zeros);
        target->WriteString(exp, exp_length);
        return num_size;
    }

    int fill_count = spec.width - num_size;

    if (spec.flags.left) {
        if (sign) target->WriteChar(sign);
        target->WriteString(prefix, prefix_length);
        target->WriteChars('0', leading_zeros);
        target->WriteString(digits, digits_length);
        target->WriteChars('0', padding_zeros);
        target->WriteString(exp, exp_length);
        target->WriteChars(' ', fill_count);
    } else {
        if (spec.flags.zero) {
            if (sign) target->WriteChar(sign);
            target->WriteString(prefix, prefix_length);
            target->WriteChars('0', fill_count + leading_zeros);
            target->WriteString(digits, digits_length);
            target->WriteChars('0', padding_zeros);
            target->WriteString(exp, exp_length);
        } else {
            target->WriteChars(' ', fill_count);
            if (sign) target->WriteChar(sign);
            target->WriteString(prefix, prefix_length);
            target->WriteChars('0', leading_zeros);
            target->WriteString(digits, digits_length);
            target->WriteChars('0', padding_zeros);
            target->WriteString(exp, exp_length);
        }
    }

    return spec.width;
}

template <int Base, typename T>
static char* UnsignedToString(T value, char* buffer, const char* digits_table)
{
    char* p = buffer;
    do {
        *p = digits_table[value % Base];
        value /= Base;
        ++p;
    } while (value != 0);
    std::reverse(buffer, p);
    return p;
}

int WriteVoidPtr(FormatPrintTarget* target, const PrintSpecification& spec,
                 const void* value)
{
    if (!spec.SpecifierMatch('p')) {
        LOG(DFATAL) << "Invalid scan specifier '" << spec.specifier
                    << "' for pointer type";
        return 0;
    }

    if (value == NULL) {
        return WriteString(target, spec, "(null)", 6);
    }

    char buf[32];
    int n = snprintf(buf, sizeof(buf), "%p", value);
    return WriteString(target, spec, buf, n);
}

int PrintValueWriter<bool>::Write(FormatPrintTarget* target,
                                  const PrintSpecification& spec,
                                  const bool* value)
{
    if (!spec.SpecifierMatch('b')) {
        LOG(DFATAL) << "Invalid print specifier '" << spec.specifier
                    << "' for bool type";
        return 0;
    }
    if (*value)
        return WriteString(target, spec, "true", 4);
    else
        return WriteString(target, spec, "false", 5);
}

int PrintValueWriter<char>::Write(FormatPrintTarget* target,
                                  const PrintSpecification& spec,
                                  const char* value)
{
    if (!spec.SpecifierMatch('c')) {
        LOG(DFATAL) << "Invalid print specifier '" << spec.specifier
                    << "' for char type";
        return 0;
    }
    return WriteString(target, spec, value, 1);
}

int PrintValueWriter<const char*>::Write(FormatPrintTarget* target,
                                         const PrintSpecification& spec,
                                         const char* value)
{
    if (!spec.SpecifierMatch("sp")) {
        LOG(DFATAL) << "Invalid print specifier '" << spec.specifier
                    << "' for const char* type";
        return 0;
    }

    if (spec.specifier == 'p')
        return WriteVoidPtr(target, spec, value);

    int length = spec.has_precision() ? strnlen(value, spec.precision) :
                                        strlen(value);
    return WriteString(target, spec, value, length);
}

int PrintValueWriter<std::string>::Write(FormatPrintTarget* target,
                                         const PrintSpecification& spec,
                                         const std::string* value)
{
    if (!spec.SpecifierMatch('s')) {
        LOG(DFATAL) << "Invalid print specifier '" << spec.specifier
                    << "' for string type";
        return 0;
    }
    int length = value->length();
    if (spec.has_precision() && spec.precision < length)
        length = spec.precision;
    return WriteString(target, spec, value->data(), length);
}

static inline const char* PrefixOf(const PrintSpecification& spec, bool is_zero)
{
    if (!is_zero && spec.flags.sharp) {
        switch (spec.specifier) {
        case 'o':
            return "0";
        case 'x':
            return "0x";
        case 'X':
            return "0X";
        }
    }
    return "";
}

static inline char SignOf(const PrintSpecification& spec, bool neg)
{
    if (neg)
        return '-';
    if (spec.flags.sign)
        return '+';
    if (spec.flags.space)
        return ' ';
    return '\0';
}

static unsigned int ToUnsigned(int n)
{
    return static_cast<unsigned int>(n);
}

static unsigned long ToUnsigned(long n)
{
    return static_cast<unsigned long>(n);
}

static unsigned long long ToUnsigned(long long n)
{
    return static_cast<unsigned long long>(n);
}

template <typename Type>
static int WriteSigned(FormatPrintTarget* target,
                       const PrintSpecification& spec, Type value)
{
    if (!spec.SpecifierMatch("idxXo")) {
        LOG(DFATAL) << "Invalid print specifier '" << spec.specifier
                    << "' for signed integral type";
        return -1;
    }

    char buf[64];
    char* digits = buf;
    int n = 0;
    switch (spec.specifier) {
    case 'i':
    case 'd':
    case 'v':
        n = WriteIntegerToBuffer(value, buf) - buf;
        if (buf[0] == '-') {
            ++digits;
            --n;
        }
        break;
    case 'x':
        n = UnsignedToString<16>(ToUnsigned(value), buf, "0123456789abcdef") - buf;
        break;
    case 'X':
        n = UnsignedToString<16>(ToUnsigned(value), buf, "0123456789ABCDEF") - buf;
        break;
    case 'o':
        n = UnsignedToString<8>(ToUnsigned(value), buf, "01234567") - buf;
        break;
    }

    int leading_zeros = 0;
    if (spec.has_precision()) {
        // Special case: Output empty digits for 0 if prec is 0
        if (value == 0 && spec.precision == 0)
            n = 0;
        if (spec.precision > n)
            leading_zeros = spec.precision - n;
    }
    const char* prefix = PrefixOf(spec, value == 0);
    char sign = SignOf(spec, value < 0);
    return WriteNumberString(target, spec, sign, prefix, strlen(prefix),
                             leading_zeros, digits, n, 0, NULL, 0);
}

template <typename Type>
static int WriteUnsigned(FormatPrintTarget* target,
                         const PrintSpecification& spec, Type value)
{
    if (!spec.SpecifierMatch("uidxXo")) {
        LOG(DFATAL) << "Invalid print specifier '" << spec.specifier
                    << "' for unsigned integral type";
        return -1;
    }

    char buf[64];
    int n = 0;
    switch (spec.specifier) {
    case 'i':
    case 'd':
    case 'u':
    case 'v':
        n = WriteIntegerToBuffer(value, buf) - buf;
        break;
    case 'x':
        n = UnsignedToString<16>(value, buf, "0123456789abcdef") - buf;
        break;
    case 'X':
        n = UnsignedToString<16>(value, buf, "0123456789ABCDEF") - buf;
        break;
    case 'o':
        n = UnsignedToString<8>(value, buf, "01234567") - buf;
        break;
    }

    int leading_zeros = 0;
    if (spec.has_precision()) {
        // Special case: Output empty digits for 0 if prec is 0
        if (value == 0 && spec.precision == 0)
            n = 0;
        if (spec.precision > n)
            leading_zeros = spec.precision - n;
    }
    const char* prefix = PrefixOf(spec, value == 0);
    return WriteNumberString(target, spec, '\0', prefix, strlen(prefix),
                             leading_zeros, buf, n, 0, NULL, 0);
}

#define STRING_PRINT_DEFINE_SIGNED_INT_WRITE(Type) \
int PrintValueWriter<Type>::Write(FormatPrintTarget* target, \
                                  const PrintSpecification& spec, \
                                  const Type* value) \
{ \
    return WriteSigned(target, spec, *value); \
}

#define STRING_PRINT_DEFINE_UNSIGNED_INT_WRITE(Type) \
int PrintValueWriter<Type>::Write(FormatPrintTarget* target, \
                                  const PrintSpecification& spec, \
                                  const Type* value) \
{ \
    return WriteUnsigned(target, spec, *value); \
}

STRING_PRINT_DEFINE_SIGNED_INT_WRITE(signed char)
STRING_PRINT_DEFINE_SIGNED_INT_WRITE(short)
STRING_PRINT_DEFINE_SIGNED_INT_WRITE(int)
STRING_PRINT_DEFINE_SIGNED_INT_WRITE(long)
STRING_PRINT_DEFINE_SIGNED_INT_WRITE(long long)

STRING_PRINT_DEFINE_UNSIGNED_INT_WRITE(unsigned char)
STRING_PRINT_DEFINE_UNSIGNED_INT_WRITE(unsigned short)
STRING_PRINT_DEFINE_UNSIGNED_INT_WRITE(unsigned int)
STRING_PRINT_DEFINE_UNSIGNED_INT_WRITE(unsigned long)
STRING_PRINT_DEFINE_UNSIGNED_INT_WRITE(unsigned long long)

template <typename T>
int WriteFloat(FormatPrintTarget* target,
               const PrintSpecification& spec,
               const char* length,
               T value)
{
    if (!spec.SpecifierMatch("aefgAEFG")) {
        LOG(DFATAL) << "Invalid print specifier '" << spec.specifier
                    << "' for unsigned integral type";
        return -1;
    }

    char buf[4096];
    char specifier = spec.specifier != 'v' ? spec.specifier : 'g';
    char format[16];
    if (spec.has_precision()) {
        snprintf(format, sizeof(format), "%%.%d%s%c",
                 std::min(spec.precision, 16), length, specifier);
    } else {
        if (spec.flags.sharp)
            snprintf(format, sizeof(format), "%%.6%s%c", length, specifier);
        else
            snprintf(format, sizeof(format), "%%%s%c", length, specifier);
    }
    char* digits = buf;
    int n = snprintf(buf, sizeof(buf), format, value);
    if (buf[0] == '-') {
        ++digits;
        --n;
    }
    char sign = SignOf(spec, value < 0);
    int leading_zeros = 0;
    int padding_zeros = 0;
    return WriteNumberString(target, spec, sign, NULL, 0, leading_zeros,
                             buf, n, padding_zeros, NULL, 0);
}

int PrintValueWriter<float>::Write(FormatPrintTarget* target,
                                   const PrintSpecification& spec,
                                   const float* value)
{
    return WriteFloat(target, spec, "", *value);
}

int PrintValueWriter<double>::Write(FormatPrintTarget* target,
                                    const PrintSpecification& spec,
                                    const double* value)
{
    return WriteFloat(target, spec, "l", *value);
}

int PrintValueWriter<long double>::Write(FormatPrintTarget* target,
                                         const PrintSpecification& spec,
                                         const long double* value)
{
    return WriteFloat(target, spec, "L", *value);
}

// This function is rarely used, so don't add another function pointer to
// reduce performance of most common case.
int FormatPrintArg::AsInt() const
{
    PrintSpecification spec;
    spec.specifier = 'd';
    char buffer[32];
    { // Put t into a separate scope ensure '\0' is append after written.
        BufferFormatPrintTarget t(buffer, sizeof(buffer));
        if (Write(&t, spec) <= 0)
            return -1;
    }
    int n;
    if (!StringToNumber(buffer, &n))
        return -1;
    return n;
}

} // namespace toft
