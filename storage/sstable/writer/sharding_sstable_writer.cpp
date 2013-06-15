// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/writer/sharding_sstable_writer.h"

#include "toft/base/stl_util.h"
#include "toft/base/string/format.h"
#include "toft/base/string/number.h"
#include "toft/hash/fingerprint.h"
#include "toft/storage/sharding/sharding.h"
#include "toft/storage/sstable/writer/composited_sstable_writer.h"

#include "thirdparty/glog/logging.h"

namespace toft {

const std::string ShardingSSTableWriter::GetShardingPath(const std::string &path,
                                                         int shard_index,
                                                         int shard_num) {
    return StringPrint("%s-%05d-of-%05d", path.c_str(), shard_index, shard_num);
}

ShardingSSTableWriter::ShardingSSTableWriter(int shard_num, const SSTableWriteOption &option)
                : SSTableWriter(option),
                  shard_num_(shard_num),
                  set_id_(0) {
    std::string policy = option.sharding_policy();
    sharding_policy_.reset(TOFT_CREATE_SHARDING_POLICY(policy));
    CHECK(sharding_policy_.get()) << "sharding policy is invalid: " << policy;
    sharding_policy_->SetShardingNumber(shard_num_);

    std::string set_finger = StringPrint("%s/%ld", option.path().c_str(), time(NULL));
    set_id_ = Fingerprint64(set_finger);
    VLOG(1) << "set_id_:" << set_id_;

    for (int i = 0; i < shard_num_; ++i) {
        SSTableWriteOption opt(option);
        opt.set_path(GetShardingPath(option.path(), i, shard_num_));
        builders_.push_back(new CompositedSSTableWriter(opt));
    }
}

ShardingSSTableWriter::~ShardingSSTableWriter() {
    DeleteElements(&builders_);
}

bool ShardingSSTableWriter::Add(const std::string &key, const std::string &value) {
    int index = sharding_policy_->Shard(key);
    CHECK_GE(index, 0) << "bad index, key:" << key;
    return builders_[index]->Add(key, value);
}

void ShardingSSTableWriter::AddMetaData(const std::string &key, const std::string &value) {
    file_info_meta_[key] = value;
}

bool ShardingSSTableWriter::Flush() {
    SetMetaData();
    bool result = true;
    for (int i = 0; i < shard_num_; ++i) {
        if (!builders_[i]->Flush())
            result = false;
    }
    return result;
}

void ShardingSSTableWriter::SetMetaData() {
    std::string total_shard = NumberToString(shard_num_);
    for (int i = 0; i < shard_num_; ++i) {
        for (std::map<std::string, std::string>::const_iterator it = file_info_meta_.begin();
                        it != file_info_meta_.end(); ++it)
            builders_[i]->AddMetaData(it->first, it->second);

        builders_[i]->AddMetaData(kShardID, NumberToString(i));
        builders_[i]->AddMetaData(kShardTotalNum, total_shard);
        builders_[i]->AddMetaData(kShardPolicy, option_.sharding_policy());
        builders_[i]->AddMetaData(kSSTableSetID, NumberToString(set_id_));
    }
}

}  // namespace toft
