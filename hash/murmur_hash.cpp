// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/hash/murmur_hash.h"

#include <string>

#include "thirdparty/stringencoders/modp_b16.h"

namespace {
static const uint64_t kFingerPrintSeed = 19820125;
}

namespace toft {

uint64_t MurmurHash64A(const std::string& buffer, uint32_t seed) {
    return MurmurHash64A(buffer.data(), buffer.size(), seed);
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

uint64_t Fingerprint(const std::string& str) {
    return MurmurHash64A(str, kFingerPrintSeed);
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
}  // namespace base
