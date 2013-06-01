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

#include "toft/hash/hash.h"

#include <string.h>
#include <string>
#include "thirdparty/stringencoders/modp_b16.h"
#include "toft/base/string/algorithm.h"

namespace {
static const uint32_t kFingerPrint32Seed = 0xfd12deff;
static const uint32_t kFingerPrintSeed0 = 0x6d6f;
static const uint32_t kFingerPrintSeed1 = 0x7a63;
static const uint64_t kFingerPrintSeed = 19820125;
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
}  //  NOLINT

uint32_t Fingerprint32(const std::string &key) {
  return Fingerprint32WithSeed(key.data(), key.size(),
                               kFingerPrint32Seed);
}

uint32_t Fingerprint32(const char *str, size_t length) {
  return Fingerprint32WithSeed(str, length,
                               kFingerPrint32Seed);
}

uint32_t Fingerprint32(const char *str) {
  return Fingerprint32WithSeed(str, strlen(str),
                               kFingerPrint32Seed);
}

uint32_t Fingerprint32WithSeed(const std::string &key,
                                   uint32_t seed) {
  return Fingerprint32WithSeed(key.data(), key.size(),
                               seed);
}

uint32_t Fingerprint32WithSeed(const char *str,
                                   size_t length,
                                   uint32_t seed) {
  uint32_t len = static_cast<uint32_t>(length);
  uint32_t a = 0x9e3779b9;
  uint32_t b = a;
  uint32_t c = seed;

  while (len >= 12) {
    a += (str[0] + ((uint32_t)str[1] << 8) + ((uint32_t)str[2] << 16)  //  NOLINT
          + ((uint32_t)str[3] << 24));  //  NOLINT
    b += (str[4] + ((uint32_t)str[5] << 8) + ((uint32_t)str[6] << 16)  //  NOLINT
          + ((uint32_t)str[7] << 24));  //  NOLINT
    c += (str[8] + ((uint32_t)str[9] << 8) + ((uint32_t)str[10] << 16)  //  NOLINT
          + ((uint32_t)str[11] << 24));  //  NOLINT
    mix(a, b, c);
    str += 12;
    len -= 12;
  }

  c += static_cast<uint32_t>(length);
  switch (len) {
    case 11: c += ((uint32_t)str[10] << 24);  //  NOLINT
    case 10: c += ((uint32_t)str[9] << 16);  //  NOLINT
    case 9 : c += ((uint32_t)str[8] << 8);  //  NOLINT
    case 8 : b += ((uint32_t)str[7] << 24);  //  NOLINT
    case 7 : b += ((uint32_t)str[6] << 16);  //  NOLINT
    case 6 : b += ((uint32_t)str[5] << 8);  //  NOLINT
    case 5 : b += str[4];
    case 4 : a += ((uint32_t)str[3] << 24);  //  NOLINT
    case 3 : a += ((uint32_t)str[2] << 16);  //  NOLINT
    case 2 : a += ((uint32_t)str[1] << 8);  //  NOLINT
    case 1 : a += str[0];
  }
  mix(a, b, c);

  return c;
}

uint32_t Fingerprint32WithSeed(const char *str,
                                   uint32_t seed) {
  return Fingerprint32WithSeed(str, strlen(str), seed);
}

uint32_t JenkinsOneAtATimeHash(const std::string& str) {
  return JenkinsOneAtATimeHash(str.data(), str.size());
}

//  uint32_t_t jenkins_one_at_a_time_hash(char *key, size_t len)
uint32_t JenkinsOneAtATimeHash(const char *key, size_t len) {
  uint32_t hash, i;
  for (hash = i = 0; i < len; ++i) {
    hash += key[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}

uint64_t Fingerprint(const std::string& str) {
  return MurmurHash64A(str.data(),
                       str.size(),
                       kFingerPrintSeed);
}

std::string FingerprintToString(uint64_t fp) {
  return modp::b16_encode(reinterpret_cast<char*>(&fp), sizeof(uint64_t));
}

uint64_t StringToFingerprint(const std::string& str) {
  std::string tmp = modp::b16_decode(str);
  uint64_t value = 0;
  memcpy(&value, tmp.data(), sizeof(uint64_t));
  return value;
}

// 64-bit hash for 64-bit platforms
uint64_t MurmurHash64A(const void* key, int len, uint32_t seed) {
  const uint64_t m = 0xc6a4a7935bd1e995;
  const int r = 47;

  uint64_t h = seed ^ (len * m);

  const uint64_t* data = (const uint64_t *)key;
  const uint64_t* end = data + (len/8);

  while (data != end) {
    uint64_t k = *data++;

    k *= m;
    k ^= k >> r;
    k *= m;

    h ^= k;
    h *= m;
  }

  const uint8_t* data2 = (const uint8_t*)data;

  switch (len & 7) {
    case 7: h ^= static_cast<uint64_t>(data2[6]) << 48;
    case 6: h ^= static_cast<uint64_t>(data2[5]) << 40;
    case 5: h ^= static_cast<uint64_t>(data2[4]) << 32;
    case 4: h ^= static_cast<uint64_t>(data2[3]) << 24;
    case 3: h ^= static_cast<uint64_t>(data2[2]) << 16;
    case 2: h ^= static_cast<uint64_t>(data2[1]) << 8;
    case 1: h ^= static_cast<uint64_t>(data2[0]);
    h *= m;
  };

  h ^= h >> r;
  h *= m;
  h ^= h >> r;

  return h;
}

}  // namespace base
