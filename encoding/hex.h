// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_ENCODING_HEX_H
#define TOFT_ENCODING_HEX_H

/// @file
/// @author Chen Feng <chen3feng@gmail.com>
/// @date Jan 12, 2011

#include <iterator>
#include <string>

namespace toft {

struct Hex {
private:
    Hex();
    ~Hex();

public:
    /// @brief execute hex encoding
    /// @tparam ForwardIterator forward iterator
    /// @tparam OutputIterator output iterator
    /// @param first start of encoding range
    /// @param last end of encoding range
    /// @param uppercase whether yield upper case result
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

    /// @brief hex encoding, append result to string
    /// @tparam Container any STL compatible container that support push_back
    /// @param data data to be encoded
    /// @param size data size
    /// @param output buffer to output
    /// @param uppercase whether yield upper case result
    /// @return *output as Container&
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

    /// @brief hex encoding, output result to string(overwrite)
    /// @tparam Container any STL compatible container that support push_back
    /// @param data data to be encoded
    /// @param size data size
    /// @param output buffer to output
    /// @param uppercase whether yield upper case result
    /// @return *output as Container&
    template <typename Container>
    static Container& EncodeTo(
        const void* data, size_t size,
        Container* output,
        bool uppercase = false)
    {
        output->clear();
        return EncodeAppend(data, size, output, uppercase);
    }

    /// @brief hex encoding to buffer
    /// @param data data to be encoded
    /// @param size data size
    /// @param output output buffer
    /// @param uppercase whether yield upper case result
    /// @return output buffer
    /// @note output buffer size must be large enough, at lease 2 * size + 1

    static char* EncodeToBuffer(
        const void* data, size_t size,
        char* output,
        bool uppercase = false);

    /// @brief hex encoding, and return result as string
    /// @param data data to be encoded
    /// @param size data size
    /// @param uppercase whether yield upper case result
    /// @return encoded result as string
    static std::string EncodeAsString(
        const void* data, size_t size,
        bool uppercase = false);
};

} // namespace toft

#endif // TOFT_ENCODING_HEX_H
