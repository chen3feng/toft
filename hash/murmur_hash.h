// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_HASH_MURMUR_HASH_H_
#define TOFT_HASH_MURMUR_HASH_H_

#include <stdint.h>

#include <string>

namespace toft {

//  See http://en.wikipedia.org/wiki/MurmurHash for algorithm detail
uint64_t MurmurHash64A(const std::string& buffer, uint32_t seed);
uint64_t MurmurHash64A(const void* key, int len, uint32_t seed);

//  High level wrapper using MurmurHash64A
uint64_t Fingerprint(const std::string& str);

//  Helper functions for Fingerprint, it's shorter than IntegerToString,
//  So it's better to use it as key of sstable (For example , int64 can be used
//  as docid, and docid is key of sstable for many appcations).
std::string FingerprintToString(uint64_t);
uint64_t StringToFingerprint(const std::string& str);
}  // namespace toft

#endif  // TOFT_HASH_MURMUR_HASH_H_
