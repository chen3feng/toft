// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_WRITER_SHARDING_SSTABLE_WRITER_H
#define TOFT_STORAGE_SSTABLE_WRITER_SHARDING_SSTABLE_WRITER_H

#include <map>
#include <string>
#include <vector>

#include "toft/base/scoped_ptr.h"
#include "toft/storage/sstable/writer/base_sstable_writer.h"

namespace toft {
class ShardingPolicy;

class ShardingSSTableWriter : public SSTableWriter {
    TOFT_DECLARE_UNCOPYABLE(ShardingSSTableWriter);

public:
    ShardingSSTableWriter(int shard_num, const SSTableWriteOption &option);

    ~ShardingSSTableWriter();

    static const std::string GetShardingPath(const std::string &path, int shard_index,
                                             int shard_num);

    virtual bool Add(const std::string &key, const std::string &value);

    virtual void AddMetaData(const std::string &key, const std::string &value);

    virtual bool Flush();

    void SetMetaData();

private:
    int shard_num_;
    int set_id_;
    toft::scoped_ptr<ShardingPolicy> sharding_policy_;
    std::vector<SSTableWriter*> builders_;
    std::map<std::string, std::string> file_info_meta_;
};

}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_WRITER_SHARDING_SSTABLE_WRITER_H
