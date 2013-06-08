// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/compress/block/block_compression.h"

namespace toft {
BlockCompression::BlockCompression() {}

BlockCompression::~BlockCompression() {}

bool BlockCompression::Compress(const char* str, size_t length, std::string* out) {
    return DoCompress(str, length, out);
}

bool BlockCompression::Compress(StringPiece sp, std::string* out) {
    return DoCompress(sp.data(), sp.size(), out);
}

bool BlockCompression::Uncompress(const char* str, size_t length, std::string* out) {
    return DoUncompress(str, length, out);
}

bool BlockCompression::Uncompress(StringPiece sp, std::string* out) {
    return DoUncompress(sp.data(), sp.size(), out);
}
}  // namespace toft
