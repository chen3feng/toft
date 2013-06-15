// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/hash/hash.h"
#include "toft/base/benchmark.h"

const std::string test_str = "qwertyuiopasdfghjklmnbvcz";

static void CityHash32(int n) {
    for (int i = 0; i < n; i++) {
        toft::CityHash32(test_str);
    }
}

static void CityHash64(int n) {
    for (int i = 0; i < n; i++) {
        toft::CityHash64(test_str);
    }
}

static void CityHash128(int n) {
    for (int i = 0; i < n; i++) {
        toft::CityHash128(test_str);
    }
}

static void Fingerprint64(int n) {
    for (int i = 0; i < n; i++) {
        toft::Fingerprint64(test_str);
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

static void MurmurHash64A(int n) {
    for (int i = 0; i < n; i++) {
        toft::MurmurHash64A(test_str, 0);
    }
}

static void MurmurHash64B(int n) {
    for (int i = 0; i < n; i++) {
        toft::MurmurHash64B(test_str.data(), test_str.size(), 0);
    }
}

static void CRC32(int n) {
    for (int i = 0; i < n; i++) {
        toft::CRC32(test_str);
    }
}

TOFT_BENCHMARK(CityHash32)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(CityHash64)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(CityHash128)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(Fingerprint64)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(Fingerprint32)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(JenkinsOneAtATimeHash)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(SuperFastHash)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(MurmurHash64A)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(MurmurHash64B)->ThreadRange(1, NumCPUs());
TOFT_BENCHMARK(CRC32)->ThreadRange(1, NumCPUs());

