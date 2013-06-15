// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-07

#ifndef TOFT_BASE_STRING_FORMAT_PRINT_TARGETS_H
#define TOFT_BASE_STRING_FORMAT_PRINT_TARGETS_H
#pragma once

#include <string>

#include "toft/base/string/format/print_target.h"

namespace toft {

// Print to std::string
class StringFormatPrintTarget : public FormatPrintTarget {
public:
    explicit StringFormatPrintTarget(std::string* str) : m_str(str) {}
    virtual void WriteChar(char c) {
        m_str->push_back(c);
    }
    virtual void DoWriteChars(char c, int count) {
        m_str->append(count, c);
    }
    virtual void DoWriteString(const char* str, int size) {
        m_str->append(str, size);
    }
private:
    std::string* m_str;
};

// Print to a fixed buffer
class BufferFormatPrintTarget : public FormatPrintTarget {
public:
    BufferFormatPrintTarget(char* buffer, int size) :
        m_buffer(buffer), m_size(size), m_pos(0) {}
    ~BufferFormatPrintTarget();
    virtual void WriteChar(char c);
    virtual void DoWriteChars(char c, int count);
    virtual void DoWriteString(const char* str, int size);
private:
    int FreeSize() const;
    int WritableSize(int expected) const;
private:
    char* m_buffer;
    int m_size;
    int m_pos;
};

} // namespace toft

#endif // TOFT_BASE_STRING_FORMAT_PRINT_TARGETS_H
