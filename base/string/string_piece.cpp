// Copyright (c) 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: wilsonh@google.com (Wilson Hsieh)
//

#include "toft/base/string/string_piece.h"

#include <limits.h>
#include <string.h>
#include <algorithm>

#include "toft/base/string/compare.h"

namespace toft {

// defined in implementation only for shorter typing
typedef StringPiece::size_type size_type;

#ifndef _WIN32
// don't define on windows
const size_type StringPiece::npos;
#endif

bool operator==(const StringPiece& x, const StringPiece& y)
{
    return ((x.size() == y.size()) && MemoryEqual(x.data(), y.data(), x.size()));
}

int StringPiece::compare(const StringPiece& x) const {
#if 0
    return CompareByteString(m_ptr, m_length, x.m_ptr, x.m_length);
#else // unoptimized code
    int r = memcmp(m_ptr, x.m_ptr, m_length < x.m_length ? m_length : x.m_length);
    if (r != 0)
        return r;
    if (m_length < x.m_length)
        return -1;
    else if (m_length > x.m_length)
        return 1;
    return 0;
#endif
}

int StringPiece::ignore_case_compare(const StringPiece& x) const {
    int r = memcasecmp(m_ptr, x.m_ptr, m_length < x.m_length ? m_length : x.m_length);
    if (r != 0)
        return r;
    if (m_length < x.m_length)
        return -1;
    else if (m_length > x.m_length)
        return 1;
    return 0;
}

bool StringPiece::ignore_case_equal(const StringPiece& other) const {
    return size() == other.size() && memcasecmp(data(), other.data(), size()) == 0;
}

// Does "this" start with "x"
bool StringPiece::starts_with(const StringPiece& x) const {
    return ((m_length >= x.m_length) && MemoryEqual(m_ptr, x.m_ptr, x.m_length));
}

// Does "this" end with "x"
bool StringPiece::ends_with(const StringPiece& x) const {
    return ((m_length >= x.m_length) &&
            (MemoryEqual(m_ptr + m_length - x.m_length, x.m_ptr, x.m_length)));
}

size_type StringPiece::copy(char* buf, size_type n, size_type pos) const {
    size_type ret = std::min(m_length - pos, n);
    memcpy(buf, m_ptr + pos, ret);
    return ret;
}

size_type StringPiece::find(const StringPiece& s, size_type pos) const {
    if (pos > m_length)
        return npos;

    const char* result = std::search(m_ptr + pos, m_ptr + m_length,
                                     s.m_ptr, s.m_ptr + s.m_length);
    const size_type xpos = result - m_ptr;
    return xpos + s.m_length <= m_length ? xpos : npos;
}

size_type StringPiece::find(char c, size_type pos) const {
    if (pos >= m_length)
        return npos;

    const void* result = memchr(m_ptr + pos, c, m_length - pos);
    if (result)
        return reinterpret_cast<const char*>(result) - m_ptr;
    return npos;
}

size_type StringPiece::rfind(const StringPiece& s, size_type pos) const {
    if (m_length < s.m_length)
        return npos;

    if (s.empty())
        return std::min(m_length, pos);

    const char* last = m_ptr + std::min(m_length - s.m_length, pos) + s.m_length;
    const char* result = std::find_end(m_ptr, last, s.m_ptr, s.m_ptr + s.m_length);
    return result != last ? static_cast<size_t>(result - m_ptr) : npos;
}

size_type StringPiece::rfind(char c, size_type pos) const {
    if (m_length == 0)
        return npos;

    for (size_type i = std::min(pos, m_length - 1); ; --i) {
        if (m_ptr[i] == c)
            return i;
        if (i == 0)
            break;
    }
    return npos;
}

// For each character in characters_wanted, sets the index corresponding
// to the ASCII code of that character to 1 in table.  This is used by
// the m_find.*_of methods below to tell whether or not a character is in
// the lookup table in constant time.
// The argument `table' must be an array that is large enough to hold all
// the possible values of an unsigned char.  Thus it should be be declared
// as follows:
//   bool table[UCHAR_MAX + 1]
static inline void BuildLookupTable(const StringPiece& characters_wanted,
                                    bool* table) {
    const size_type length = characters_wanted.length();
    const char* const data = characters_wanted.data();
    for (size_type i = 0; i < length; ++i) {
        table[static_cast<unsigned char>(data[i])] = true;
    }
}

size_type StringPiece::find_first_of(const StringPiece& s,
                                     size_type pos) const {
    if (m_length == 0 || s.m_length == 0)
        return npos;

    // Avoid the cost of BuildLookupTable() for a single-character search.
    if (s.m_length == 1)
        return find_first_of(s.m_ptr[0], pos);

    bool lookup[UCHAR_MAX + 1] = { false };
    BuildLookupTable(s, lookup);
    for (size_type i = pos; i < m_length; ++i) {
        if (lookup[static_cast<unsigned char>(m_ptr[i])]) {
            return i;
        }
    }
    return npos;
}

size_type StringPiece::find_first_not_of(const StringPiece& s,
                                         size_type pos) const {
    if (m_length == 0)
        return npos;

    if (s.m_length == 0)
        return 0;

    // Avoid the cost of BuildLookupTable() for a single-character search.
    if (s.m_length == 1)
        return find_first_not_of(s.m_ptr[0], pos);

    bool lookup[UCHAR_MAX + 1] = { false };
    BuildLookupTable(s, lookup);
    for (size_type i = pos; i < m_length; ++i) {
        if (!lookup[static_cast<unsigned char>(m_ptr[i])]) {
            return i;
        }
    }
    return npos;
}

size_type StringPiece::find_first_not_of(char c, size_type pos) const {
    if (m_length == 0)
        return npos;

    for (; pos < m_length; ++pos) {
        if (m_ptr[pos] != c) {
            return pos;
        }
    }
    return npos;
}

size_type StringPiece::find_last_of(const StringPiece& s, size_type pos) const {
    if (m_length == 0 || s.m_length == 0)
        return npos;

    // Avoid the cost of BuildLookupTable() for a single-character search.
    if (s.m_length == 1)
        return find_last_of(s.m_ptr[0], pos);

    bool lookup[UCHAR_MAX + 1] = { false };
    BuildLookupTable(s, lookup);
    for (size_type i = std::min(pos, m_length - 1); ; --i) {
        if (lookup[static_cast<unsigned char>(m_ptr[i])])
            return i;
        if (i == 0)
            break;
    }
    return npos;
}

size_type StringPiece::find_last_not_of(const StringPiece& s,
                                        size_type pos) const {
    if (m_length == 0)
        return npos;

    size_type i = std::min(pos, m_length - 1);
    if (s.m_length == 0)
        return i;

    // Avoid the cost of BuildLookupTable() for a single-character search.
    if (s.m_length == 1)
        return find_last_not_of(s.m_ptr[0], pos);

    bool lookup[UCHAR_MAX + 1] = { false };
    BuildLookupTable(s, lookup);
    for (; ; --i) {
        if (!lookup[static_cast<unsigned char>(m_ptr[i])])
            return i;
        if (i == 0)
            break;
    }
    return npos;
}

size_type StringPiece::find_last_not_of(char c, size_type pos) const {
    if (m_length == 0)
        return npos;

    for (size_type i = std::min(pos, m_length - 1); ; --i) {
        if (m_ptr[i] != c)
            return i;
        if (i == 0)
            break;
    }
    return npos;
}

StringPiece StringPiece::substr(size_type pos, size_type n) const {
    if (pos > m_length)
        pos = m_length;
    if (n > m_length - pos)
        n = m_length - pos;
    return StringPiece(m_ptr + pos, n);
}

} // namespace toft
