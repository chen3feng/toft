// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_ENCODING_BASE64_H
#define TOFT_ENCODING_BASE64_H
#pragma once

#include <string>
#include "toft/base/string/string_piece.h"

namespace toft {

class Base64
{
public:
    // Encodes the input string in base64.  Returns true if successful and false
    // otherwise.  The output string is only modified if successful.
    static bool Encode(const StringPiece& input, std::string* output);

    // Same as above, but use urlsafe character set.
    // See rfc4648: http://tools.ietf.org/html/rfc4648
    static bool WebSafeEncode(const StringPiece& input, std::string* output);

    // Decodes the base64 input string.  Returns true if successful and false
    // otherwise.  The output string is only modified if successful.
    static bool Decode(const StringPiece& input, std::string* output);

    // Same as above, but decode the result of WebSafeEncode.
    // See rfc4648: http://tools.ietf.org/html/rfc4648
    static bool WebSafeDecode(const StringPiece& input, std::string* output);
};

} // namespace toft

#endif // TOFT_ENCODING_BASE64_H

