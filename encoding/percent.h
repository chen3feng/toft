// Copyright 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_ENCODING_PERCENT_H
#define TOFT_ENCODING_PERCENT_H

#include <ctype.h>
#include <stdint.h>
#include <string>
#include "toft/base/string/string_piece.h"

namespace toft {

/// @brief percent encoding, majorly for url
/// @see http://en.wikipedia.org/wiki/Percent-encoding
struct PercentEncoding {
public:
    // Same as escape in javascript

    /// @brief 编码追加输出到 output
    static void EncodeAppend(const StringPiece& input, std::string* output);

    /// @brief 编码输出到 output
    static void EncodeTo(const StringPiece& input, std::string* output);

    /// @brief 编码自身，替换原来的内容
    static void Encode(std::string *str);

    /// @brief 返回编码后的结果
    static std::string Encode(const StringPiece& input);

    // Same as encodeURI in javascript

    /// @brief 编码追加输出到 output
    static void EncodeUriAppend(const StringPiece& input, std::string* output);

    /// @brief 编码输出到 output
    static void EncodeUriTo(const StringPiece& input, std::string* output);

    /// @brief 编码自身，替换原来的内容
    static void EncodeUri(std::string *str);

    /// @brief 返回编码后的结果
    static std::string EncodeUri(const StringPiece& input);

    // Same as encodeURIComponent in javascript

    /// @brief 编码追加输出到 output
    static void EncodeUriComponentAppend(const StringPiece& input, std::string* output);

    /// @brief 编码输出到 output
    static void EncodeUriComponentTo(const StringPiece& input, std::string* output);

    /// @brief 编码自身，替换原来的内容
    static void EncodeUriComponent(std::string *str);

    /// @brief 返回编码后的结果
    static std::string EncodeUriComponent(const StringPiece& input);


    /// @brief 解码后，追加方式输出到 output 里
    static bool DecodeAppend(const StringPiece& input, std::string* output);

    /// @brief 解码后，输出到 output 里
    /// @return 是否成功
    static bool DecodeTo(const StringPiece& input, std::string* output);

    /// @brief 解码自身，替换原来的内容
    static bool Decode(std::string* str);
};

} // namespace toft

#endif // TOFT_ENCODING_PERCENT_H
