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

//  NOTE: Code got from
//  https://code.google.com/p/mozc/source/browse/trunk/src/base/hash.cc

#ifndef TOFT_HASH_FINGERPRINT32_H_
#define TOFT_HASH_FINGERPRINT32_H_

#include <stdint.h>
#include <string>

#include "toft/hash/jenkins_hash.h"
#include "toft/hash/murmur_hash.h"

namespace toft {
uint32_t Fingerprint32(const std::string &key);
uint32_t Fingerprint32(const char *str, size_t length);

uint32_t Fingerprint32WithSeed(const std::string &key, uint32_t seed);
uint32_t Fingerprint32WithSeed(const char *str, size_t length, uint32_t seed);

}  //  namespace base
#endif  // TOFT_HASH_FINGERPRINT32_H_
