// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_HFILE_FILE_INFO_H
#define TOFT_STORAGE_SSTABLE_HFILE_FILE_INFO_H

#include <stdint.h>

#include <string>
#include <utility>
#include <vector>

#include "toft/storage/sstable/hfile/block.h"

namespace toft {
namespace hfile {

class FileInfo : public Block {
    TOFT_DECLARE_UNCOPYABLE(FileInfo);

public:
    FileInfo();
    ~FileInfo();
    // Save parsed meta
    std::vector<std::pair<std::string, std::string> > meta_items;

    virtual const std::string EncodeToString() const;
    virtual bool DecodeFromString(const std::string &str);
    void AddItem(const std::string &key, const std::string &value);

    // Getters and Setters
    int32_t item_num() const {
        return item_num_;
    }
    std::string last_key() const {
        return last_key_;
    }
    int32_t avg_key_len() const {
        return avg_key_len_;
    }
    int32_t avg_value_len() const {
        return avg_value_len_;
    }
    std::string comparator() const {
        return comparator_;
    }
    void set_item_num(int32_t item_num) {
        item_num_ = item_num;
    }
    void set_last_key(std::string last_key) {
        last_key_ = last_key;
    }
    void set_avg_key_len(int32_t avg_key_len) {
        avg_key_len_ = avg_key_len;
    }
    void set_avg_value_len(int32_t avg_value_len) {
        avg_value_len_ = avg_value_len;
    }
    void set_comparator(std::string comparator) {
        comparator_ = comparator;
    }

private:
    static const std::string RESERVED_PREFIX;
    static const std::string LASTKEY;
    static const std::string AVG_KEY_LEN;
    static const std::string AVG_VALUE_LEN;
    static const std::string COMPARATOR;

    // Item num in this list
    int32_t item_num_;
    // Last key of data blocks
    std::string last_key_;
    // Average length of data keys
    int32_t avg_key_len_;
    // Average length of data values
    int32_t avg_value_len_;
    // Comparator class name of data keys
    std::string comparator_;
    // Save input meta data
    std::string buffer_;
};

}  // namespace hfile
}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_HFILE_FILE_INFO_H
