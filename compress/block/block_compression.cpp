// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/compress/block/block_compression.h"

namespace {
const size_t kMaxUnCompressedSize = 20 * 1024 * 1024;  // 20M
}

namespace toft {
BlockCompression::BlockCompression() : max_unCompressed_size_(kMaxUnCompressedSize) {}

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
