// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/hash/hash.h"
#include "thirdparty/benchmark/benchmark.h"

const std::string test_str = "qwertyuiopasdfghjklmnbvcz";

static void CityHash32(benchmark::State& state) {
    for (auto _ : state) {
        toft::CityHash32(test_str);
    }
}

static void CityHash64(benchmark::State& state) {
    for (auto _ : state) {
        toft::CityHash64(test_str);
    }
}

static void CityHash128(benchmark::State& state) {
    for (auto _ : state) {
        toft::CityHash128(test_str);
    }
}

static void Fingerprint64(benchmark::State& state) {
    for (auto _ : state) {
        toft::Fingerprint64(test_str);
    }
}

static void Fingerprint32(benchmark::State& state) {
    for (auto _ : state) {
        toft::Fingerprint32(test_str);
    }
}

static void JenkinsOneAtATimeHash(benchmark::State& state) {
    for (auto _ : state) {
        toft::JenkinsOneAtATimeHash(test_str);
    }
}

static void SuperFastHash(benchmark::State& state) {
    for (auto _ : state) {
        toft::SuperFastHash(test_str);
    }
}

static void MurmurHash64A(benchmark::State& state) {
    for (auto _ : state) {
        toft::MurmurHash64A(test_str, 0);
    }
}

static void MurmurHash64B(benchmark::State& state) {
    for (auto _ : state) {
        toft::MurmurHash64B(test_str.data(), test_str.size(), 0);
    }
}

static void CRC32(benchmark::State& state) {
    for (auto _ : state) {
        toft::CRC32::Digest(test_str);
    }
}

BENCHMARK(CityHash32)->ThreadRange(1, 4);
BENCHMARK(CityHash64)->ThreadRange(1, 4);
BENCHMARK(CityHash128)->ThreadRange(1, 4);
BENCHMARK(Fingerprint64)->ThreadRange(1, 4);
BENCHMARK(Fingerprint32)->ThreadRange(1, 4);
BENCHMARK(JenkinsOneAtATimeHash)->ThreadRange(1, 4);
BENCHMARK(SuperFastHash)->ThreadRange(1, 4);
BENCHMARK(MurmurHash64A)->ThreadRange(1, 4);
BENCHMARK(MurmurHash64B)->ThreadRange(1, 4);
BENCHMARK(CRC32)->ThreadRange(1, 4);

