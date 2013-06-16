// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/hfile/data_index.h"

#include "toft/base/string/algorithm.h"
#include "toft/base/string/format.h"
#include "toft/encoding/varint.h"
#include "toft/storage/sstable/hfile/coding.h"

namespace toft {
namespace hfile {

static const std::string kIndexBlockMagic = "IDXBLK\41\43";

DataIndex::DataIndex() : last_offset_(0) {
}

DataIndex::~DataIndex() {
}

bool DataIndex::DecodeFromString(const std::string &str) {
    if (!StringStartsWith(str, kIndexBlockMagic)) {
        LOG(ERROR)<< "invalid data index header";
        return false;
    }
    block_info_.clear();
    const char *begin = str.c_str() + kIndexBlockMagic.size();
    const char *end = str.c_str() + str.size();
    while (begin < end) {
        DataBlockInfo info;
        info.offset = ReadInt64(&begin);
        info.data_size = ReadInt32(&begin);
        int key_len = ReadVint(&begin, end);
        info.key = std::string(begin, key_len);
        begin += key_len;
        block_info_.push_back(info);
    }
    // Check if the content is overflow
    if (begin > end) {
        LOG(ERROR) << "incomplete file, "
        << StringPrint("begin: %p, end: %p", begin, end);
        return false;
    }
    return true;
}

void DataIndex::AddDataBlockInfo(int compress_data_size, int uncompress_data_size,
                                 const std::string &first_key) {
    if (buffer_.empty())
        buffer_ = kIndexBlockMagic;
    PutFixed64(&buffer_, last_offset_);
    PutFixed32(&buffer_, uncompress_data_size);
    Varint::Put32(&buffer_, first_key.size());
    buffer_ += first_key;
    last_offset_ += compress_data_size;
}

int DataIndex::FindMinimalBlock(const std::string &key) const {
    int begin = 0;
    int end = block_info_.size() - 1;
    int mid = 0;
    while (begin <= end) {
        mid = (begin + end) / 2;
        const std::string &cur_key = block_info_[mid].key;
        VLOG(4) << "begin: " << begin << "; mid: " << mid << "; end: " << end;
        if (cur_key < key) {
            begin = mid + 1;
        } else {
            end = mid - 1;
        }
    }
    const std::string &cur_key = block_info_[mid].key;

    if (cur_key < key) {
        // The binary search result is what we found
        return mid;
    } else {
        // Return the previous block
        if (mid > 0)
            --mid;
        return mid;
    }
}

}  // namespace hfile
}  // namespace toft
