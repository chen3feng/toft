// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/encoding/base64.h"

#include "stringencoders/modp_b64.h"
#include "stringencoders/modp_b64r.h"
#include "stringencoders/modp_b64w.h"

namespace toft {

bool Base64::Encode(const StringPiece& input, std::string* output)
{
    std::string temp;
    temp.resize(modp_b64_encode_len(input.size()));  // makes room for null byte

    // null terminates result since result is base64 text!
    int input_size = static_cast<int>(input.size());
    int output_size = modp_b64_encode(&(temp[0]), input.data(), input_size);
    if (output_size < 0)
        return false;

    temp.resize(output_size);  // strips off null byte
    output->swap(temp);
    return true;
}

bool Base64::WebSafeEncode(const StringPiece& input, std::string* output)
{
    std::string temp;
    temp.resize(modp_b64r_encode_len(input.size()));  // makes room for null byte

    // null terminates result since result is base64 text!
    int input_size = static_cast<int>(input.size());
    int output_size = modp_b64r_encode(&(temp[0]), input.data(), input_size);
    if (output_size < 0)
        return false;

    temp.resize(output_size);  // strips off null byte
    output->swap(temp);
    return true;
}

bool Base64::Decode(const StringPiece& input, std::string* output)
{
    std::string temp;
    temp.resize(modp_b64_decode_len(input.size()));

    // does not null terminate result since result is binary data!
    int input_size = static_cast<int>(input.size());
    int output_size = modp_b64_decode(&(temp[0]), input.data(), input_size);
    if (output_size < 0)
        return false;

    temp.resize(output_size);
    output->swap(temp);
    return true;
}

bool Base64::WebSafeDecode(const StringPiece& input, std::string* output)
{
    std::string temp;
    temp.resize(modp_b64r_decode_len(input.size()));

    // does not null terminate result since result is binary data!
    int input_size = static_cast<int>(input.size());
    int output_size = modp_b64r_decode(&(temp[0]), input.data(), input_size);
    if (output_size < 0)
        return false;

    temp.resize(output_size);
    output->swap(temp);
    return true;
}

} // namespace toft
