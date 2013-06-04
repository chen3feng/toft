// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Authors: CHEN Feng <chen3feng@gmail.com>

// Reencapsulation of google cityhash.
// * Add namespace
// * Using stdint
// * Not using pair as int128

#ifndef TOFT_HASH_CITY_H
#define TOFT_HASH_CITY_H
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string>
#include "toft/base/int128.h"

namespace toft {

// Hash function for a byte array.
uint64_t CityHash64(const void *buf, size_t len);

// Hash function for a string.
inline uint64_t CityHash64(const std::string& str) {
    return CityHash64(str.data(), str.size());
}

// Hash function for a byte array.  For convenience, a 64-bit seed is also
// hashed into the result.
uint64_t CityHash64WithSeed(const void *buf, size_t len, uint64_t seed);

// Hash function for a byte array.  For convenience, two seeds are also
// hashed into the result.
uint64_t CityHash64WithSeeds(const void *buf, size_t len,
                             uint64_t seed0, uint64_t seed1);

// Hash function for a byte array.
UInt128 CityHash128(const void *s, size_t len);

// Hash function for a string.
inline UInt128 CityHash128(const std::string& str) {
    return CityHash128(str.data(), str.size());
}

// Hash function for a byte array.  For convenience, a 128-bit seed is also
// hashed into the result.
UInt128 CityHash128WithSeed(const void *s, size_t len, UInt128 seed);

// Hash function for a byte array.  Most useful in 32-bit binaries.
uint32_t CityHash32(const void *buf, size_t len);

// Hash function for a string.
inline uint32_t CityHash32(const std::string& str) {
    return CityHash32(str.data(), str.size());
}

} // namespace toft

#endif // TOFT_HASH_CITY_H
