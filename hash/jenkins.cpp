// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/hash/jenkins.h"

#include <string>

namespace toft {

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

}  // namespace toft
