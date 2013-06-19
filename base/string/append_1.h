// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-01-28
// Description: StringAppend with 1 extra parameter

#ifndef TOFT_BASE_STRING_APPEND_1_H
#define TOFT_BASE_STRING_APPEND_1_H
#pragma once

// GLOBAL_NOLINT(runtime/int)

#ifndef TOFT_BASE_STRING_CONCAT_H
#error "This file is an implemention detail of string concat, you should not include it directly"
#endif

#include <stdio.h>
#include <ostream>
#include <streambuf>
#include <string>

#include "toft/base/static_assert.h"
#include "toft/base/type_traits.h"

namespace toft {

/////////////////////////////////////////////////////////////////////////////
// Optimize for common types

inline void StringAppend(std::string* str, const std::string& value)
{
    str->append(value);
}

inline void StringAppend(std::string* str, const char* value)
{
    str->append(value);
}

inline void StringAppend(std::string* str, char value)
{
    str->push_back(value);
}

inline void StringAppend(std::string* str, bool value)
{
    if (value)
        str->append("true", 4);
    else
        str->append("false", 5);
}

void StringAppend(std::string* str, short value);
void StringAppend(std::string* str, unsigned short value);
void StringAppend(std::string* str, int value);
void StringAppend(std::string* str, unsigned int value);
void StringAppend(std::string* str, long value);
void StringAppend(std::string* str, unsigned long value);
void StringAppend(std::string* str, long long value);
void StringAppend(std::string* str, unsigned long long value);
void StringAppend(std::string* str, float value);
void StringAppend(std::string* str, double value);
void StringAppend(std::string* str, long double value);

namespace internal {

// Direct write to string to reduce memory copy.
class string_streambuf : public std::streambuf {
public:
    explicit string_streambuf(std::string* str) : m_str(str) {}
    virtual std::streamsize xsputn(const char * s, std::streamsize n) {
        m_str->append(s, n);
        return n;
    }
private:
    std::string* m_str;
};

template <bool is_enum>
struct StringAppendEnum
{
    template <typename T>
    static void Append(std::string* str, const T& value)
    {
        string_streambuf buf(str);
        std::ostream oss(&buf);
        oss.exceptions(std::ios_base::failbit | std::ios_base::badbit);
        oss << value;
    }
};

// Optimize for enums, enums are integers,
template <>
struct StringAppendEnum<true>
{
private:
    static void AppendEnum(std::string* str, int value)
    {
        StringAppend(str, value);
    }

    static void AppendEnum(std::string* str, unsigned int value)
    {
        StringAppend(str, value);
    }

    static void AppendEnum(std::string* str, long value)
    {
        StringAppend(str, value);
    }

    static void AppendEnum(std::string* str, unsigned long value)
    {
        StringAppend(str, value);
    }

    static void AppendEnum(std::string* str, long long value)
    {
        StringAppend(str, value);
    }

    static void AppendEnum(std::string* str, unsigned long long value)
    {
        StringAppend(str, value);
    }

public:
    template <typename T>
    static void Append(std::string* str, const T& value)
    {
        AppendEnum(str, value);
    }
};

/////////////////////////////////////////////////////////////////////////////
// GuessStringSize is used to reserve memory befor append

// Expect 4 to be average length for such as integers.
template <typename T>
inline size_t GuessStringSize(const T& o)
{
    return 4;
}

template <size_t N>
inline size_t GuessStringSize(const char(&str)[N])  // NOLINT(readability/casting)
{
    return N - 1;
}

inline size_t GuessStringSize(const std::string& str)
{
    return str.size();
}

} // namespace internal

// Default way to append
template <typename T>
void StringAppend(std::string* str, const T& value)
{
    // Convert signed char or unsigned char to string is ambiguous
    // may be typedefed to int8_t or uint8_t
    // disable them
    TOFT_STATIC_ASSERT((!std::is_same<T, signed char>::value),
        "signed char is not allowed");
    TOFT_STATIC_ASSERT((!std::is_same<T, unsigned char>::value),
        "unsigned char is not allowed");
    internal::StringAppendEnum<std::is_enum<T>::value>::Append(str, value);
}

} // namespace toft

#endif // TOFT_BASE_STRING_APPEND_1_H
