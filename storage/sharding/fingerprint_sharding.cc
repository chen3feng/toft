// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sharding/fingerprint_sharding.h"

#include "toft/hash/fingerprint.h"

namespace toft {

FingerprintSharding::FingerprintSharding() {
}

FingerprintSharding::~FingerprintSharding() {
}

int FingerprintSharding::Shard(const std::string& key) {
    int shard_id = Fingerprint64(key) % (shard_num_);
    return shard_id;
}

TOFT_REGISTER_SHARDING_POLICY(FingerprintSharding);
}  // namespace util
