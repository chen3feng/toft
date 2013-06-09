// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/encoding/hex.h"

namespace toft {

char* Hex::EncodeToBuffer(
    const void* data, size_t size,
    char* output,
    bool uppercase)
{
    const unsigned char* p = static_cast<const unsigned char*>(data);
    Encode(p, p + size, output, uppercase);
    output[2 * size] = '\0';
    return output;
}

std::string Hex::EncodeAsString(
    const void* data, size_t size,
    bool uppercase)
{
    std::string str;
    EncodeTo(data, size, &str, uppercase);
    return str;
}

} // namespace toft
