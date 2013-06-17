// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/hfile/file_trailer.h"

#include "toft/base/string/algorithm.h"
#include "toft/storage/sstable/hfile/coding.h"

#include "thirdparty/glog/logging.h"

namespace {
static const std::string kTrailerBlockMagic = "TRABLK\34\36";
}

namespace toft {
namespace hfile {

FileTrailer::FileTrailer()
                : file_info_offset_(-1),
                  data_index_offset_(-1),
                  data_index_count_(0),
                  meta_index_offset_(0),
                  meta_index_count_(0),
                  total_uncompressed_bytes_(0),
                  entry_count_(0),
                  compress_type_(CompressType_kUnCompress),
                  version_(kCurrentVersion) {
}

FileTrailer::~FileTrailer() {
}

int FileTrailer::TrailerSize() {
    return sizeof(int32_t) * 5 + sizeof(int64_t) * 4 + kTrailerBlockMagic.size();
}

const std::string FileTrailer::EncodeToString() const {
    std::string result(kTrailerBlockMagic);
    PutFixed64(&result, file_info_offset_);
    PutFixed64(&result, data_index_offset_);
    PutFixed32(&result, data_index_count_);
    PutFixed64(&result, meta_index_offset_);
    PutFixed32(&result, meta_index_count_);
    PutFixed64(&result, total_uncompressed_bytes_);
    PutFixed32(&result, entry_count_);
    PutFixed32(&result, compress_type_);
    PutFixed32(&result, version_);
    return result;
}

bool FileTrailer::DecodeFromString(const std::string &str) {
    if (str.size() != static_cast<std::string::size_type>(TrailerSize())) {
        LOG(ERROR)<< "error size: " << str.size();
        return false;
    }
    if (!StringStartsWith(str, kTrailerBlockMagic)) {
        LOG(ERROR) << "invalid block magic:" << str;
        return false;
    }
    const char *begin = str.c_str() + kTrailerBlockMagic.size();
    file_info_offset_ = ReadInt64(&begin);
    data_index_offset_ = ReadInt64(&begin);
    data_index_count_ = ReadInt32(&begin);
    meta_index_offset_ = ReadInt64(&begin);
    meta_index_count_ = ReadInt32(&begin);
    total_uncompressed_bytes_ = ReadInt64(&begin);
    entry_count_ = ReadInt32(&begin);
    compress_type_ =
    static_cast<CompressType>(ReadInt32(&begin));
    version_ = ReadInt32(&begin);
    VLOG(10) << "trailer size: " << str.size()
             << ", file info offset: " << file_info_offset_
             << ", data index offset: " << data_index_offset_
             << ", data index count: " << data_index_count_;
    return true;
}

}  // namespace hfile
}  // namespace toft
