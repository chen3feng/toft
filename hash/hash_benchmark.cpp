// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/hash/hash.h"
#include "toft/base/benchmark.h"

const std::string test_str = "qwertyuiopasdfghjklmnbvcz";

static void Fingerprint(int n) {
    for (int i = 0; i < n; i++) {
        toft::Fingerprint(test_str);
    }
}

static void Fingerprint32(int n) {
    for (int i = 0; i < n; i++) {
        toft::Fingerprint32(test_str);
    }
}

static void JenkinsOneAtATimeHash(int n) {
    for (int i = 0; i < n; i++) {
        toft::JenkinsOneAtATimeHash(test_str);
    }
}

static void SuperFastHash(int n) {
    for (int i = 0; i < n; i++) {
        toft::SuperFastHash(test_str);
    }
}

TOFT_BENCHMARK(Fingerprint)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(Fingerprint32)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(JenkinsOneAtATimeHash)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(SuperFastHash)->ThreadRange(1, NumCPUs());

