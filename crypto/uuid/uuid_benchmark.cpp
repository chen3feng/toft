// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/crypto/uuid/uuid.h"
#include "thirdparty/benchmark/benchmark.h"

#include "thirdparty/glog/logging.h"

static void CreateCanonicalUUIDString(benchmark::State& state) {
    for (auto _ : state) {
        std::string uuid = toft::CreateCanonicalUUIDString();
    }
}

BENCHMARK(CreateCanonicalUUIDString)->ThreadRange(1, 4);
