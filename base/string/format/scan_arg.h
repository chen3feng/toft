// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-05

#ifndef TOFT_BASE_STRING_FORMAT_SCAN_ARG_H
#define TOFT_BASE_STRING_FORMAT_SCAN_ARG_H
#pragma once

#include <limits.h>
#include <string>

#include "toft/base/string/format/specification.h"

// GLOBAL_NOLINT(runtime/int)
// GLOBAL_NOLINT(readability/function)

namespace toft {

template <typename T> class ScanValueParser {};

#define STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(Type) \
template <> \
class ScanValueParser<Type> { \
public: \
    static int Parse(const char* str, const ScanSpecification& spec, Type* value); \
}

// For normal types
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(bool);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(char);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(signed char);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(unsigned char);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(short);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(unsigned short);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(int);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(unsigned int);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(long);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(unsigned long);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(long long);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(unsigned long long);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(float);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(double);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(long double);
STRING_SCAN_DECLARE_VALUE_PARSER_SPECIALIZATION(std::string);

////////////////////////////////////////////////////////////////////////////////
// For pointer

int ParseVoidPtr(const char* str, const ScanSpecification& spec, void** value);

template <typename T>
class ScanValueParser<T*> {
public:
    static int Parse(const char* str, const ScanSpecification& spec, T** value)
    {
        return ParseVoidPtr(str, spec, reinterpret_cast<void**>(value));
    }
};

class FormatScanArg {
    typedef int (*ParseFunction)(const char* str, const ScanSpecification& spec, void* value);

public:
    template <typename T>
    FormatScanArg(T* p) : // NOLINT(runtime/explicit)
        m_ptr(p),
        m_parse(reinterpret_cast<ParseFunction>(&ScanValueParser<T>::Parse))
    {
    }

    int Parse(const char* buf, const ScanSpecification& spec) const;
    static int ParseSkipped(const char* string, const ScanSpecification& spec);
    bool WriteInt(int n, const ScanSpecification& spec) const;

private:
    void* m_ptr;
    ParseFunction m_parse;
};

} // namespace toft

#endif // TOFT_BASE_STRING_FORMAT_SCAN_ARG_H
