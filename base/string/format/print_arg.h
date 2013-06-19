// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-07

#ifndef TOFT_BASE_STRING_FORMAT_PRINT_ARG_H
#define TOFT_BASE_STRING_FORMAT_PRINT_ARG_H
#pragma once

#include <stddef.h>
#include <string>

#include "toft/base/string/format/print_target.h"
#include "toft/base/string/format/specification.h"

// GLOBAL_NOLINT(readability/function)

namespace toft {

template <typename T> class PrintValueWriter {
public:
    // A specialization must contains this member:
    // static int Write(FormatPrintTarget* target,
    //                  const PrintSpecification& spec,
    //                  const Type* value);
};

#define STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(Type) \
template <> \
class PrintValueWriter<Type> { \
public: \
    static int Write(FormatPrintTarget* target, \
                     const PrintSpecification& spec, \
                     Type const* value); \
}

// For normal types
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(bool);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(char);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(signed char);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(unsigned char);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(short);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(unsigned short);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(int);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(unsigned int);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(long);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(unsigned long);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(long long);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(unsigned long long);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(float);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(double);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(long double);
STRING_PRINT_DECLARE_VALUE_WRITER_SPECIALIZATION(std::string);

template <>
class PrintValueWriter<const char*> {
public:
    static int Write(FormatPrintTarget* target,
                     const PrintSpecification& spec,
                     const char* value);
};

////////////////////////////////////////////////////////////////////////////////
// For pointer

int WriteVoidPtr(FormatPrintTarget* target,
                 const PrintSpecification& spec,
                 const void* value);

template <typename T>
class PrintValueWriter<T*> {
public:
    static int Write(FormatPrintTarget* target,
                     const PrintSpecification& spec,
                     const T** value)
    {
        return WriteVoidPtr(target, spec, *value);
    }
};

class FormatPrintArg {
    typedef int (*WriteFunction)(FormatPrintTarget* target,
                                 const PrintSpecification& spec,
                                 const void* value);

public:
    template <typename T>
    FormatPrintArg(const T& v) : // NOLINT(runtime/explicit)
        m_ptr(&v),
        m_write(reinterpret_cast<WriteFunction>(&PrintValueWriter<T>::Write))
    {
    }

    // Accept const char[], char [], char*, const char*
    FormatPrintArg(const char* v) : // NOLINT(runtime/explicit)
        m_ptr(v),
        m_write(reinterpret_cast<WriteFunction>(&PrintValueWriter<const char*>::Write))
    {
    }

    int Write(FormatPrintTarget* target, const PrintSpecification& spec) const
    {
        return m_write(target, spec, m_ptr);
    }

    // Return the value as integer, used for when width and precision is '*'.
    // Return -1 if can't convert to int.
    int AsInt() const;

private:
    const void* const m_ptr;
    WriteFunction m_write;
};

} // namespace toft

#endif // TOFT_BASE_STRING_FORMAT_PRINT_ARG_H
