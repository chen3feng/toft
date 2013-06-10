// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: Jan 12, 2011

#ifndef TOFT_ENCODING_HEX_H
#define TOFT_ENCODING_HEX_H

#include <stddef.h>
#include <iterator>
#include <string>

namespace toft {

// TODO(chen3feng): Add Decode functions.
struct Hex {
private:
    Hex();
    ~Hex();

public:
    // Encode [first, last) sequence to output iterator.
    template <typename ForwardIterator, typename OutputIterator>
    static void Encode(
        ForwardIterator first,
        ForwardIterator last,
        OutputIterator output,
        bool uppercase = false)
    {
        const char* hex_digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
        while (first != last)
        {
            unsigned char ch = *first;
            *output++ = hex_digits[ch >> 4];
            *output++ = hex_digits[ch & 0x0F];
            ++first;
        }
    }

    // Encode to STL-like containers has push_back such as vector/string
    // without clear.
    template <typename Container>
    static Container& EncodeAppend(
        const void* data, size_t size,
        Container* output,
        bool uppercase = false)
    {
        const unsigned char* p = static_cast<const unsigned char*>(data);
        Encode(p, p + size, std::back_inserter(*output), uppercase);
        return *output;
    }

    // Encode to STL-like containers with push_back method, such as vector/string.
    // Previous content will be replaced.
    template <typename Container>
    static Container& EncodeTo(
        const void* data, size_t size,
        Container* output,
        bool uppercase = false)
    {
        output->clear();
        return EncodeAppend(data, size, output, uppercase);
    }

    // Write to C style char buffer with terminal '\0'.
    // Caller should ensure output is large enough.
    static char* EncodeToBuffer(
        const void* data, size_t size,
        char* output,
        bool uppercase = false);

    // Encode As a C++ string.
    static std::string EncodeAsString(
        const void* data, size_t size,
        bool uppercase = false);
};

} // namespace toft

#endif // TOFT_ENCODING_HEX_H
