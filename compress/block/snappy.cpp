// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/compress/block/snappy.h"

#include "snappy/snappy.h"

namespace toft {
SnappyCompression::SnappyCompression() {}

SnappyCompression::~SnappyCompression() {}

bool SnappyCompression::DoCompress(const char* str, size_t length, std::string* out) {
    snappy::Compress(str, length, out);
    return true;
}

bool SnappyCompression::DoUncompress(const char* str, size_t length, std::string* out) {
    return snappy::Uncompress(str, length, out);
}

TOFT_REGISTER_BLOCK_COMPRESSION(SnappyCompression, "snappy");
}  // namespace toft
