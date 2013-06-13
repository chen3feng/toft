// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef UTIL_SHARDING_SHARDING_H_
#define UTIL_SHARDING_SHARDING_H_

#include "toft/base/uncopyable.h"
#include "toft/base/class_registry/class_registry.h"

namespace toft {
class ShardingPolicy {
 public:
  ShardingPolicy();
  virtual ~ShardingPolicy();

  virtual void SetShardingNumber(int shard_num) {
    shard_num_ = shard_num;
  }

  virtual int Shard(const std::string& key) = 0;

 protected:
  int shard_num_;

 private:
  TOFT_DECLARE_UNCOPYABLE(ShardingPolicy);
};

TOFT_CLASS_REGISTRY_DEFINE(sharding_policy_registry, ShardingPolicy);

#define TOFT_REGISTER_SHARDING_POLICY(class_name) \
    TOFT_CLASS_REGISTRY_REGISTER_CLASS( \
        toft::sharding_policy_registry, \
        toft::ShardingPolicy, \
        #class_name, \
        class_name)
}  // namespace toft

#define TOFT_CREATE_SHARDING_POLICY(name_as_string) \
    TOFT_CLASS_REGISTRY_CREATE_OBJECT(toft::sharding_policy_registry, name_as_string)

#endif  // UTIL_SHARDING_SHARDING_H_

