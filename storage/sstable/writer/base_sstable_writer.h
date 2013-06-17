// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_WRITER_BASE_SSTABLE_WRITER_H
#define TOFT_STORAGE_SSTABLE_WRITER_BASE_SSTABLE_WRITER_H

#include <string>

#include "toft/base/uncopyable.h"
#include "toft/storage/sstable/types.h"

namespace toft {
namespace hfile {
class ShardingPolicy;
class DataBlock;
class DataIndex;
} // namespace hfile

class SSTableWriter {
    TOFT_DECLARE_UNCOPYABLE(SSTableWriter);

public:
    explicit SSTableWriter(const SSTableWriteOption &option);
    virtual ~SSTableWriter() {
    }

    //  For key/value data
    virtual bool Add(const std::string &key, const std::string &value);
    void AddOrDie(const std::string &key, const std::string &value);

    // Add File Info Meta, value length less than 1k
    virtual void AddMetaData(const std::string &key, const std::string &value) {
    }

    std::string GetTempSSTablePath(const std::string &path);
    bool MoveToRealPath(const std::string &path);

    // Called after all key/value pairs are added
    // Return true if all sstables are built on disk successfully
    // otherwise return false
    // Do NOT call this method twice.
    virtual bool Flush() {
        return false;
    }

protected:
    SSTableWriteOption option_;
};
}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_WRITER_BASE_SSTABLE_WRITER_H
