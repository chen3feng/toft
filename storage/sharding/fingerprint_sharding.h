// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SHARDING_FINGER_SHARDING_H
#define TOFT_STORAGE_SHARDING_FINGER_SHARDING_H

#include <string>

#include "toft/storage/sharding/sharding.h"

namespace toft {
class FingerprintSharding : public ShardingPolicy {
    TOFT_DECLARE_UNCOPYABLE(FingerprintSharding);

public:
    FingerprintSharding();
    virtual ~FingerprintSharding();

    virtual int Shard(const std::string& key);
};
}  // namespace util
#endif  // TOFT_STORAGE_SHARDING_FINGER_SHARDING_H
