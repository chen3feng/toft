// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/hfile/file_info.h"

#include "toft/encoding/varint.h"
#include "toft/storage/sstable/hfile/coding.h"

namespace toft {
namespace hfile {

const std::string FileInfo::RESERVED_PREFIX = "hfile.";
const std::string FileInfo::AVG_KEY_LEN =  FileInfo::RESERVED_PREFIX
    + "AVG_KEY_LEN";
const std::string FileInfo::AVG_VALUE_LEN = FileInfo::RESERVED_PREFIX
    + "AVG_VALUE_LEN";
const std::string FileInfo::COMPARATOR = FileInfo::RESERVED_PREFIX
    + "COMPARATOR";
const std::string FileInfo::LASTKEY = FileInfo::RESERVED_PREFIX
    + "LASTKEY";

FileInfo::FileInfo()
    : item_num_(4),
      last_key_(""),
      avg_key_len_(0),
      avg_value_len_(0),
      comparator_("org.apache.hadoop.hbase.util.Bytes$ByteArrayComparator") {
}

FileInfo::~FileInfo() {
}

const std::string FileInfo::EncodeToString() const {
    std::string result;
    PutFixed32(&result, item_num_);
    Varint::Put32(&result, AVG_KEY_LEN.length());
    result += AVG_KEY_LEN;
    result += "\1";  // for cmpatible with HFile
    Varint::Put32(&result, sizeof(int32_t));
    PutFixed32(&result, avg_key_len_);
    Varint::Put32(&result, AVG_VALUE_LEN.length());
    result += AVG_VALUE_LEN;
    result += "\1";
    Varint::Put32(&result, sizeof(int32_t));
    PutFixed32(&result, avg_value_len_);
    Varint::Put32(&result, COMPARATOR.length());
    result += COMPARATOR;
    result += "\1";
    Varint::Put32(&result, comparator_.length());
    result += comparator_;
    Varint::Put32(&result, LASTKEY.length());
    result += LASTKEY;
    result += "\1";
    Varint::Put32(&result, last_key_.length());
    result += last_key_;
    return result + buffer_;
}

bool FileInfo::DecodeFromString(const std::string &str) {
    const char *begin = str.c_str();
    item_num_ = ReadInt32(&begin);
    const char *end = str.c_str() + str.length();
    while (begin < end) {
        int key_length = ReadVint(&begin, end);
        std::string key = std::string(begin, key_length);
        begin += key_length + 1;  // read one byte more, about HFile format
        int value_length = ReadVint(&begin, end);
        if (key == AVG_KEY_LEN) {
            avg_key_len_ = ReadInt32(&begin);
            continue;
        } else if (key == AVG_VALUE_LEN) {
            avg_value_len_ = ReadInt32(&begin);
            continue;
        } else if (key == COMPARATOR) {
            comparator_ = std::string(begin, value_length);
            begin += value_length;
            continue;
        } else if (key == LASTKEY) {
            last_key_ = std::string(begin, value_length);
            begin += value_length;
            continue;
        }
        std::string value = std::string(begin, value_length);
        begin += value_length;
        meta_items.push_back(make_pair(key, value));
    }
    return true;
}

void FileInfo::AddItem(const std::string &key, const std::string &value) {
    Varint::Put32(&buffer_, key.length());
    buffer_ += key;
    buffer_ += "\1";
    Varint::Put32(&buffer_, value.length());
    buffer_ += value;
    ++item_num_;
}

}  // namespace hfile
}  // namespace toft
