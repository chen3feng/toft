// Copyright (c) 2011, The Toft Authors
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/encoding/percent.h"
#include <iso646.h>
#include "toft/base/string/byte_set.h"
#include "toft/encoding/ascii.h"

namespace toft {

///////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////

static inline char CharToHex(uint8_t x)
{
    return "0123456789ABCDEF"[x];
}

static inline int HexValue(uint8_t x)
{
    uint8_t X = Ascii::ToUpper(x);
    return Ascii::IsDigit(X) ? X-'0' : X-'A'+10;
}

static void DoEncodeAppend(
    const StringPiece& input,
    const ByteSet& unchanged,
    std::string* output)
{
    for (size_t i = 0; i < input.size(); ++i) {
        if (unchanged.Find((uint8_t)input[i]))
        {
            output->push_back(input[i]);
        } else {
            output->push_back('%');
            output->push_back(CharToHex((uint8_t)input[i] >> 4));
            output->push_back(CharToHex((uint8_t)input[i] % 16));
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// encode
///////////////////////////////////////////////////////////////////////////

void PercentEncoding::EncodeAppend(const StringPiece& input, std::string* output)
{
    static const ByteSet unchanged("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz"
                                   "0123456789*+-./@_");
    DoEncodeAppend(input, unchanged, output);
}

void PercentEncoding::EncodeTo(const StringPiece& input, std::string* output)
{
    output->clear();
    EncodeAppend(input, output);
}

std::string PercentEncoding::Encode(const StringPiece& input) {
    std::string result;
    EncodeAppend(input, &result);
    return result;
}

void PercentEncoding::Encode(std::string *str)
{
    std::string tmp;
    EncodeAppend(*str, &tmp);
    std::swap(*str, tmp);
}

// Encode URI

void PercentEncoding::EncodeUriAppend(const StringPiece& input, std::string* output)
{
    static const ByteSet unchanged("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz"
                                   "0123456789!#$&'()*+,-./:;=?@_~");
    DoEncodeAppend(input, unchanged, output);
}

void PercentEncoding::EncodeUriTo(const StringPiece& input, std::string* output)
{
    output->clear();
    EncodeUriAppend(input, output);
}

std::string PercentEncoding::EncodeUri(const StringPiece& input) {
    std::string result;
    EncodeUriAppend(input, &result);
    return result;
}

void PercentEncoding::EncodeUri(std::string *str)
{
    std::string tmp;
    EncodeUriAppend(*str, &tmp);
    std::swap(*str, tmp);
}

// Encode URI Component

void PercentEncoding::EncodeUriComponentAppend(const StringPiece& input, std::string* output)
{
    static const ByteSet unchanged("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz"
                                   "0123456789!'()*-._~");
    DoEncodeAppend(input, unchanged, output);
}

void PercentEncoding::EncodeUriComponentTo(const StringPiece& input, std::string* output)
{
    output->clear();
    EncodeUriComponentAppend(input, output);
}

std::string PercentEncoding::EncodeUriComponent(const StringPiece& input) {
    std::string result;
    EncodeUriComponentAppend(input, &result);
    return result;
}

void PercentEncoding::EncodeUriComponent(std::string *str)
{
    std::string tmp;
    EncodeUriComponentAppend(*str, &tmp);
    std::swap(*str, tmp);
}

///////////////////////////////////////////////////////////////////////////////
// decode
///////////////////////////////////////////////////////////////////////////////

bool PercentEncoding::DecodeAppend(const StringPiece& input, std::string* output)
{
    for (size_t i = 0; i < input.size(); ++i) {
        uint8_t ch = 0;
        if (input[i] == '%') {
            if (i + 2 > input.size()) {
                // 后面的数据不完整了，返回吧
                return false;
            }

            if (!Ascii::IsHexDigit(input[i+1]) || !Ascii::IsHexDigit(input[i+2]))
                return false;

            ch = (HexValue(input[i+1]) << 4);
            ch |= HexValue(input[i+2]);
            i += 2;
        } else if (input[i] == '+') {
            ch = ' ';
        } else {
            ch = input[i];
        }
        *output += static_cast<char>(ch);
    }
    return true;
}

bool PercentEncoding::DecodeTo(const StringPiece& input, std::string* output)
{
    output->clear();
    return DecodeAppend(input, output);
}

bool PercentEncoding::Decode(std::string *str)
{
    std::string& s = *str;
    size_t write_pos = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        uint8_t ch = 0;
        if (s[i] == '%') {
            if (i + 2 > s.size()) {
                // 后面的数据不完整了，返回吧
                return false;
            }

            if (!Ascii::IsHexDigit(s[i+1]) || !Ascii::IsHexDigit(s[i+2]))
                return false;

            ch = (HexValue(s[i+1]) << 4);
            ch |= HexValue(s[i+2]);
            i += 2;
        } else if (s[i] == '+') {
            ch = ' ';
        } else {
            ch = s[i];
        }
        s[write_pos++] = static_cast<char>(ch);
    }
    s.resize(write_pos);
    return true;
}

} // namespace toft

