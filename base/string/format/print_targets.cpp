// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-07

#include "toft/base/string/format/print_targets.h"

#include <string.h>
#include <algorithm>

namespace toft {

BufferFormatPrintTarget::~BufferFormatPrintTarget()
{
    if (m_pos < m_size)
        m_buffer[m_pos++] = '\0';
}

inline int BufferFormatPrintTarget::FreeSize() const
{
    return m_size > 0 ? m_size - m_pos - 1 : 0;
}

inline int BufferFormatPrintTarget::WritableSize(int expected) const
{
    return std::min(FreeSize(), expected);
}

void BufferFormatPrintTarget::WriteChar(char c)
{
    if (FreeSize() > 0)
        m_buffer[m_pos++] = c;
}

void BufferFormatPrintTarget::DoWriteChars(char c, int count)
{
    int n = WritableSize(count);
    memset(m_buffer + m_pos, c, n);
    m_pos += n;
}

void BufferFormatPrintTarget::DoWriteString(const char* str, int size)
{
    int n = WritableSize(size);
    memcpy(m_buffer + m_pos, str, n);
    m_pos += n;
}

} // namespace toft

