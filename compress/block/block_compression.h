// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_COMPRESS_BLOCK_BLOCK_COMPRESSION_H
#define TOFT_COMPRESS_BLOCK_BLOCK_COMPRESSION_H

#include <string>

#include "toft/base/class_registry.h"
#include "toft/base/string/string_piece.h"
#include "toft/base/uncopyable.h"

namespace toft {

class BlockCompression {
    TOFT_DECLARE_UNCOPYABLE(BlockCompression);

public:
    BlockCompression();
    virtual ~BlockCompression();

    bool Compress(const char* str, size_t length, std::string* out);
    bool Compress(StringPiece sp, std::string* out);
    bool Uncompress(const char* str, size_t length, std::string* out);
    bool Uncompress(StringPiece sp, std::string* out);
    virtual std::string GetName() = 0;
    void SetMaxUnCompressedSize(size_t s) {
        max_unCompressed_size_ = s;
    }

protected:
    size_t max_unCompressed_size_;

private:
    virtual bool DoCompress(const char* str, size_t length, std::string* out) = 0;
    virtual bool DoUncompress(const char* str, size_t length, std::string* out) = 0;
};

TOFT_CLASS_REGISTRY_DEFINE(block_compression_registry, BlockCompression);

#define TOFT_REGISTER_BLOCK_COMPRESSION(class_name, algorithm_name) \
    TOFT_CLASS_REGISTRY_REGISTER_CLASS( \
        toft::block_compression_registry, \
        toft::BlockCompression, \
        algorithm_name, \
        class_name)

#define TOFT_CREATE_BLOCK_COMPRESSION(name) \
    TOFT_CLASS_REGISTRY_CREATE_OBJECT(block_compression_registry, name)

}  // namespace toft
#endif  // TOFT_COMPRESS_BLOCK_BLOCK_COMPRESSION_H
