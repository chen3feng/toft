// Copyright 2009 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// This file defines a url hash function.

#ifndef TOFT_HASH_HASH_H_
#define TOFT_HASH_HASH_H_

#include <stdint.h>
#include <string>

namespace toft {
uint32_t Fingerprint32(const std::string &key);
uint32_t Fingerprint32(const char *str, size_t length);
uint32_t Fingerprint32WithSeed(const std::string &key,
                             uint32_t seed);
uint32_t Fingerprint32WithSeed(const char *str,
                             size_t length,
                             uint32_t seed);
uint32_t Fingerprint32WithSeed(const char *str,
                             uint32_t seed);

uint64_t Fingerprint(const std::string& str);
std::string FingerprintToString(uint64_t);
uint64_t StringToFingerprint(const std::string& str);
uint64_t MurmurHash64A(const void* key, int len, uint32_t seed);

//  The Jenkins hash functions are a collection of
//  (non-cryptographic) hash functions for multi-byte
//  keys designed by Bob Jenkins. They can be used also as
//  checksums to detect accidental data corruption or detect
//  identical records in a database.
//  see http://en.wikipedia.org/wiki/Jenkins_hash_function
//  for details
uint32_t JenkinsOneAtATimeHash(const std::string& str);
uint32_t JenkinsOneAtATimeHash(const char *key, size_t len);
}  //  namespace base
#endif  // TOFT_HASH_HASH_H_
