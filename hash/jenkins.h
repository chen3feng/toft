// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_HASH_JENKINS_H_
#define TOFT_HASH_JENKINS_H_

#include <stdint.h>

#include <string>

namespace toft {

//  The Jenkins hash functions are a collection of
//  (non-cryptographic) hash functions for multi-byte
//  keys designed by Bob Jenkins. They can be used also as
//  checksums to detect accidental data corruption or detect
//  identical records in a database.
//  see http://en.wikipedia.org/wiki/Jenkins_hash_function
//  for details
uint32_t JenkinsOneAtATimeHash(const std::string& str);
uint32_t JenkinsOneAtATimeHash(const char *key, size_t len);

}  // namespace toft

#endif  // TOFT_HASH_JENKINS_H_
