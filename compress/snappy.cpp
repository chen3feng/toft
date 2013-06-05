// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/compress/snappy.h"

#include "thirdparty/snappy/snappy.h"

namespace toft {
SnappyCompression::SnappyCompression() {

}
SnappyCompression::~SnappyCompression() {

}

bool SnappyCompression::Compress(const char* str, size_t length, std::string* out) {
    snappy::Compress(str, length, out);
    return true;
}

bool SnappyCompression::Uncompress(const char* str, size_t length, std::string* out) {
    return snappy::Uncompress(str, length, out);
}

REGISTER_COMPRESSION(SnappyCompression);
}
