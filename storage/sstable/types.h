// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_TYPES_H
#define TOFT_STORAGE_SSTABLE_TYPES_H

#include <stdint.h>

#include <string>

namespace toft {

static const std::string kShardID = "shard_id";
static const std::string kShardTotalNum = "shard_total_num";
static const std::string kShardPolicy = "shard_policy";
static const std::string kSSTableSetID = "shard_set_id";

enum CompressType {
    CompressType_kLzo = 0,
    CompressType_kZlib = 1,
    CompressType_kUnCompress = 2,
    // Do NOT change above enum value, it's used in hfile of java version
    CompressType_kSnappy = 3,
    CompressType_kUnKnown
};

class SSTableWriteOption {
public:
    SSTableWriteOption()
        : compress_type_(CompressType_kUnCompress),
          block_size_(64 * 1024) {
    }

    void set_path(const std::string &path) {
        path_ = path;
    }
    const std::string path() const {
        return path_;
    }

    void set_block_size(int block_size) {
        block_size_ = block_size;
    }
    size_t block_size() const {
        return block_size_;
    }

    void set_compress_type(CompressType codec) {
        compress_type_ = codec;
    }
    int compress_type() const {
        return compress_type_;
    }

    const std::string& sharding_policy() const {
        return sharding_policy_;
    }
    void set_sharding_policy(const std::string& policy) {
        sharding_policy_ = policy;
    }

private:
    int compress_type_;
    int64_t block_size_;
    std::string path_;
    std::string sharding_policy_;
};
}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_TYPES_H
