// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_HFILE_DATA_INDEX_H
#define TOFT_STORAGE_SSTABLE_HFILE_DATA_INDEX_H

#include <string>
#include <vector>

#include "toft/storage/sstable/hfile/block.h"

#include "thirdparty/glog/logging.h"

namespace toft {
namespace hfile {

class DataIndex : public Block {
    TOFT_DECLARE_UNCOPYABLE(DataIndex);

public:
    DataIndex();
    ~DataIndex();

    virtual const std::string EncodeToString() const {
        return buffer_;
    }
    virtual bool DecodeFromString(const std::string &str);

    void AddDataBlockInfo(int compress_data_size, int uncompress_data_size,
                          const std::string &first_key);

    // Getters
    int GetBlockSize() const {
        return block_info_.size();
    }
    int64_t GetOffset(size_t index) const {
        CHECK(index < block_info_.size());
        return block_info_[index].offset;
    }
    int32_t GetDataSize(size_t index) const {
        CHECK(index < block_info_.size());
        return block_info_[index].data_size;
    }
    const std::string &GetKey(size_t index) const {
        CHECK(index < block_info_.size());
        return block_info_[index].key;
    }

    // For one key, find the minimal block that the key would probably in it.
    int FindMinimalBlock(const std::string &key) const;

private:
    struct DataBlockInfo {
        int64_t offset;
        int32_t data_size;
        std::string key;
    };
    // Save parsed data from string
    std::vector<DataBlockInfo> block_info_;
    // To save the inputed data info
    std::string buffer_;
    int64_t last_offset_;
};

}  // namespace hfile
}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_HFILE_DATA_INDEX_H
