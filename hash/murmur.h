// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Authors: Ye Shunping <yeshunping@gmail.com>
//          CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_HASH_MURMUR_H_
#define TOFT_HASH_MURMUR_H_

#include <stdint.h>

#include <string>

namespace toft {

//  See http://en.wikipedia.org/wiki/MurmurHash for algorithm detail
uint64_t MurmurHash64A(const std::string& buffer, uint64_t seed);
uint64_t MurmurHash64A(const void* key, size_t len, uint64_t seed);

// 64-bit hash for 32-bit platforms
uint64_t MurmurHash64B(const void * key, size_t len, uint64_t seed);

uint32_t MurmurHash2A(const void * key, size_t len, uint32_t seed);

void MurmurHash3_x86_32(const void * key, int len, uint32_t seed, void * out);
void MurmurHash3_x86_64(const void * key, int len, uint32_t seed, void * out);
void MurmurHash3_x86_128(const void * key, int len, uint32_t seed, void * out);

void MurmurHash3_x64_32(const void * key, int len, uint32_t seed, void * out);
void MurmurHash3_x64_64(const void * key, int len, uint32_t seed, void * out);
void MurmurHash3_x64_128(const void * key, int len, uint32_t seed, void * out);

// Same algorithm as MurmurHash2, but only does aligned reads - should be safer
// on certain platforms.
// Performance will be lower than MurmurHash2
uint32_t MurmurHashAligned2(const void * key, size_t len, uint32_t seed);

// Same as MurmurHash2, but endian- and alignment-neutral.
// Half the speed though, alas.
uint32_t MurmurHashNeutral2(const void * key, size_t len, uint32_t seed);

}  // namespace toft

#endif  // TOFT_HASH_MURMUR_H_
