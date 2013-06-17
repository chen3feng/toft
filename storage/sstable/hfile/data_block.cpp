// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/hfile/data_block.h"

#include "toft/base/string/algorithm.h"
#include "toft/base/string/format.h"
#include "toft/compress/block/block_compression.h"
#include "toft/storage/sstable/hfile/coding.h"

#include "thirdparty/glog/logging.h"

//  GLOBAL_NOLINT(runtime/sizeof)

namespace {
static const std::string kDataBlockMagic = "DATABLK\42";
}

namespace toft {
namespace hfile {

DataBlock::~DataBlock() {
}

DataBlock::DataBlock(CompressType codec)
                : compression_(NULL),
                  compressed_size_(0) {
    switch (codec) {
    case CompressType_kSnappy:
        compression_ = TOFT_CREATE_BLOCK_COMPRESSION("snappy");
        break;
    case CompressType_kLzo:
        compression_ = TOFT_CREATE_BLOCK_COMPRESSION("snappy");
        break;
    case CompressType_kUnCompress:
        break;
    default:
        LOG(FATAL)<< "not supported yet!";
    }
}

const std::string DataBlock::EncodeToString() const {
    if (compression_ != NULL) {
        std::string compressed;
        if (!compression_->Compress(buffer_.c_str(), buffer_.size(), &compressed)) {
            LOG(ERROR)<< "compress failed!";
            return "";
        }
        // save the compressed info
        compressed_size_ = compressed.size();
        return compressed;
    }
    compressed_size_ = buffer_.size();
    return buffer_;
}

bool DataBlock::DecodeFromString(const std::string &str) {
    if (compression_ != NULL) {
        std::string uncompressed;
        if (!compression_->Uncompress(str.c_str(), str.size(), &uncompressed)) {
            LOG(ERROR)<< "uncompress failed!";
            return false;
        }
        return DecodeInternal(uncompressed);
    }
    return DecodeInternal(str);
}

bool DataBlock::DecodeInternal(const std::string &str) {
    if (!StringStartsWith(str, kDataBlockMagic)) {
        LOG(INFO)<< "invalid data block header.";
        return false;
    }
    data_items_.clear();
    const char *begin = str.c_str() + kDataBlockMagic.size();
    const char *end = str.c_str() + str.length();
    while (begin < end) {
        int key_length = ReadInt32(&begin);
        int value_length = ReadInt32(&begin);
        std::string key = std::string(begin, key_length);
        begin += key_length;
        std::string value = std::string(begin, value_length);
        begin += value_length;
        data_items_.push_back(make_pair(key, value));
    }
    if (begin > end) {
        LOG(ERROR) << "not a complete data block, "
        << StringPrint("begin: %p, end: %p", begin, end);
        return false;
    }
    return true;
}

void DataBlock::AddItem(const std::string &key, const std::string &value) {
    // ignore totally empty item
    if (key.empty() && value.empty())
        return;

    if (buffer_.empty())
        buffer_ = kDataBlockMagic;

    PutFixed32(&buffer_, key.size());
    PutFixed32(&buffer_, value.size());
    buffer_ += key;
    buffer_ += value;
}

}  // namespace hfile
}  // namespace toft
