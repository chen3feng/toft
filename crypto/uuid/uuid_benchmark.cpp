// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/base/benchmark.h"
#include "toft/crypto/uuid/uuid.h"

#include "glog/logging.h"

static void CreateCanonicalUUIDString(int n) {
    for (int i = 0; i < n; i++) {
        std::string uuid = toft::CreateCanonicalUUIDString();
        VLOG(1) << uuid;
    }
}

TOFT_BENCHMARK(CreateCanonicalUUIDString)->ThreadRange(1, NumCPUs());
