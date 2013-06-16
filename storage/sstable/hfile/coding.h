// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>
//
// Endian-neutral encoding:
// * Fixed-length numbers are encoded with least-significant byte first
// * In addition we support variable length "varint" encoding
// * Strings are encoded prefixed by their length in varint format

#ifndef TOFT_STORAGE_SSTABLE_HFILE_CODING_H
#define TOFT_STORAGE_SSTABLE_HFILE_CODING_H

#include <stdint.h>
#include <string.h>
#include <string>

#include "toft/base/byte_order.h"
#include "toft/base/string/string_piece.h"

namespace toft {
namespace hfile {

void PutFixed32(std::string* dst, uint32_t value);
void PutFixed64(std::string* dst, uint64_t value);

void EncodeFixed32(char* dst, uint32_t value);
void EncodeFixed64(char* dst, uint64_t value);

int32_t ReadInt32(const char **buffer);
int64_t ReadInt64(const char **buffer);
int32_t ReadVint(const char **buffer, const char* limit);

inline uint32_t DecodeFixed32(const char* ptr) {
    if (ByteOrder::IsBigEndian()) {
        //  Load the raw bytes
        uint32_t result;
        memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
        return result;
    } else {
      return ((static_cast<uint32_t>(static_cast<unsigned char>(ptr[0]))) |
              (static_cast<uint32_t>(static_cast<unsigned char>(ptr[1])) << 8) |
              (static_cast<uint32_t>(static_cast<unsigned char>(ptr[2])) << 16) |
              (static_cast<uint32_t>(static_cast<unsigned char>(ptr[3])) << 24));
    }
}

inline uint64_t DecodeFixed64(const char* ptr) {
    if (ByteOrder::IsBigEndian()) {
        // Load the raw bytes
        uint64_t result;
        memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
        return result;
    } else {
        uint64_t lo = DecodeFixed32(ptr);
        uint64_t hi = DecodeFixed32(ptr + 4);
        return (hi << 32) | lo;
    }
}

}  // namespace hfile
}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_HFILE_CODING_H
