// Copyright 2010, Google Inc.
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

// GLOBAL_NOLINT(readability/casting)
// GLOBAL_NOLINT(whitespace/newline)

#include <string.h>

#include <string>

#include "toft/base/string/algorithm.h"
#include "toft/hash/fingerprint.h"

namespace {
static const uint32_t kFingerPrint32Seed = 0xfd12deff;
}

namespace toft {
#define mix(a, b, c) { \
    a -= b; a -= c; a ^= (c >> 13);\
    b -= c; b -= a; b ^= (a << 8);\
    c -= a; c -= b; c ^= (b >> 13);\
    a -= b; a -= c; a ^= (c >> 12);\
    b -= c; b -= a; b ^= (a << 16);\
    c -= a; c -= b; c ^= (b >> 5);\
    a -= b; a -= c; a ^= (c >> 3);\
    b -= c; b -= a; b ^= (a << 10);\
    c -= a; c -= b; c ^= (b >> 15);\
}

uint32_t Fingerprint32(const std::string &key) {
    return Fingerprint32WithSeed(key.data(), key.size(), kFingerPrint32Seed);
}

uint32_t Fingerprint32(const char *str, size_t length) {
    return Fingerprint32WithSeed(str, length, kFingerPrint32Seed);
}

uint32_t Fingerprint32WithSeed(const std::string &key, uint32_t seed) {
    return Fingerprint32WithSeed(key.data(), key.size(), seed);
}

uint32_t Fingerprint32WithSeed(const char *str, size_t length, uint32_t seed) {
    uint32_t len = static_cast<uint32_t>(length);
    uint32_t a = 0x9e3779b9;
    uint32_t b = a;
    uint32_t c = seed;

    while (len >= 12) {
        a += (str[0] + ((uint32_t) str[1] << 8) + ((uint32_t) str[2] << 16)
                        + ((uint32_t) str[3] << 24));
        b += (str[4] + ((uint32_t) str[5] << 8) + ((uint32_t) str[6] << 16)
                        + ((uint32_t) str[7] << 24));
        c += (str[8] + ((uint32_t) str[9] << 8) + ((uint32_t) str[10] << 16)
                        + ((uint32_t) str[11] << 24));
        mix(a, b, c);
        str += 12;
        len -= 12;
    }

    c += static_cast<uint32_t>(length);
    switch (len) {
    case 11:
        c += ((uint32_t) str[10] << 24);
    case 10:
        c += ((uint32_t) str[9] << 16);
    case 9:
        c += ((uint32_t) str[8] << 8);
    case 8:
        b += ((uint32_t) str[7] << 24);
    case 7:
        b += ((uint32_t) str[6] << 16);
    case 6:
        b += ((uint32_t) str[5] << 8);
    case 5:
        b += str[4];
    case 4:
        a += ((uint32_t) str[3] << 24);
    case 3:
        a += ((uint32_t) str[2] << 16);
    case 2:
        a += ((uint32_t) str[1] << 8);
    case 1:
        a += str[0];
    }
    mix(a, b, c);

    return c;
}

uint32_t Fingerprint32WithSeed(const char *str, uint32_t seed) {
    return Fingerprint32WithSeed(str, strlen(str), seed);
}

}  // namespace toft
