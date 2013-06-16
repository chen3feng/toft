// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_HFILE_DATA_BLOCK_H
#define TOFT_STORAGE_SSTABLE_HFILE_DATA_BLOCK_H

#include <string>
#include <utility>
#include <vector>

#include "toft/storage/sstable/hfile/block.h"
#include "toft/storage/sstable/types.h"

#include "thirdparty/glog/logging.h"

namespace toft {
class BlockCompression;

namespace hfile {

class DataBlock : public Block {
    TOFT_DECLARE_UNCOPYABLE(DataBlock);

public:
    explicit DataBlock(CompressType codec);
    ~DataBlock();

    virtual const std::string EncodeToString() const;
    virtual bool DecodeFromString(const std::string &str);

    // It is the caller's responsibility to keep the item ordered
    // and decide when to finish adding items
    void AddItem(const std::string &key, const std::string &value);
    void ClearItems() {
        buffer_.clear();
        compressed_size_ = 0;
    }

    int64_t GetUncompressedBufferSize() const {
        return buffer_.size();
    }
    int64_t GetCompressedBufferSize() const {
        return compressed_size_;
    }

    // Getters
    int GetDataItemSize() const {
        return data_items_.size();
    }
    const std::string GetKey(size_t index) const {
        CHECK(index < data_items_.size());
        return data_items_[index].first;
    }
    const std::string GetValue(size_t index) const {
        CHECK(index < data_items_.size());
        return data_items_[index].second;
    }

private:
    bool DecodeInternal(const std::string &str);

    BlockCompression* compression_;

    // Save parsed data from string
    std::vector<std::pair<std::string, std::string> > data_items_;
    // To save the inputed data info
    std::string buffer_;
    mutable int64_t compressed_size_;
};

}  // namespace hfile
}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_HFILE_DATA_BLOCK_H
