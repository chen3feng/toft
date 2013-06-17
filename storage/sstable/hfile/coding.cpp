// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/hfile/coding.h"

#include "toft/encoding/varint.h"

#include "thirdparty/glog/logging.h"

namespace toft {
namespace hfile {

void EncodeFixed32(char* buf, uint32_t value) {
    if (ByteOrder::IsBigEndian()) {
        memcpy(buf, &value, sizeof(value));
    } else {
        buf[0] = value & 0xff;
        buf[1] = (value >> 8) & 0xff;
        buf[2] = (value >> 16) & 0xff;
        buf[3] = (value >> 24) & 0xff;
    }
}

void EncodeFixed64(char* buf, uint64_t value) {
    if (ByteOrder::IsBigEndian()) {
        memcpy(buf, &value, sizeof(value));
    } else {
        buf[0] = value & 0xff;
        buf[1] = (value >> 8) & 0xff;
        buf[2] = (value >> 16) & 0xff;
        buf[3] = (value >> 24) & 0xff;
        buf[4] = (value >> 32) & 0xff;
        buf[5] = (value >> 40) & 0xff;
        buf[6] = (value >> 48) & 0xff;
        buf[7] = (value >> 56) & 0xff;
    }
}

void PutFixed32(std::string* dst, uint32_t value) {
    char buf[sizeof(value)];
    EncodeFixed32(buf, value);
    dst->append(buf, sizeof(buf));
}

void PutFixed64(std::string* dst, uint64_t value) {
    char buf[sizeof(value)];
    EncodeFixed64(buf, value);
    dst->append(buf, sizeof(buf));
}

int32_t ReadInt32(const char **buffer) {
    int32_t result = DecodeFixed32(*buffer);
    *buffer += sizeof(result);
    return result;
}

int64_t ReadInt64(const char **buffer) {
    int64_t result = DecodeFixed64(*buffer);
    *buffer += sizeof(result);
    return result;
}

int32_t ReadVint(const char **buffer, const char* limit) {
    uint32_t value = 0;
    const char* q = Varint::Decode32(*buffer, limit, &value);
    if (q == NULL) {
        LOG(FATAL)<< "Bad record, fail to ReadVint";
        return 0;
    } else {
        *buffer = q;
        return value;
    }
}

}  // namespace hfile
}  // namespace toft
