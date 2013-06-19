// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Authors: CHEN Feng <chen3feng@gmail.com>

#include "toft/hash/city.h"

#include "thirdparty/cityhash/city.h"

namespace toft {

static UInt128 u2U(::uint128 u) {
    return UInt128(u.second, u.first);
}

static uint128 U2u(UInt128 u) {
    return uint128(UInt128Low64(u), UInt128High64(u));
}

uint64_t CityHash64(const void *buf, size_t len) {
    return ::CityHash64(static_cast<const char*>(buf), len);
}

uint64_t CityHash64WithSeed(const void *buf, size_t len, uint64_t seed) {
    return ::CityHash64WithSeed(static_cast<const char*>(buf), len, seed);
}

uint64_t CityHash64WithSeeds(const void *buf, size_t len,
                             uint64_t seed0, uint64_t seed1) {
    return ::CityHash64WithSeeds(static_cast<const char*>(buf), len, seed0, seed1);
}

UInt128 CityHash128(const void *s, size_t len) {
    return u2U(::CityHash128(static_cast<const char*>(s), len));
}

UInt128 CityHash128WithSeed(const void *s, size_t len, UInt128 seed) {
    return u2U(::CityHash128WithSeed(static_cast<const char*>(s), len, U2u(seed)));
}

uint32_t CityHash32(const void *buf, size_t len) {
    return ::CityHash32(static_cast<const char*>(buf), len);
}

} // namespace toft
