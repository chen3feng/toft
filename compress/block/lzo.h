// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_COMPRESS_BLOCK_LZO_H
#define TOFT_COMPRESS_BLOCK_LZO_H

#include <stdint.h>

#include <string>

#include "toft/compress/block/block_compression.h"

namespace toft {

class LzoCompression : public BlockCompression {
    TOFT_DECLARE_UNCOPYABLE(LzoCompression);

public:
    LzoCompression();
    virtual ~LzoCompression();

    virtual std::string GetName() {
        return "lzo";
    }

private:
    virtual bool DoCompress(const char* str, size_t length, std::string* out);
    virtual bool DoUncompress(const char* str, size_t length, std::string* out);

    unsigned char* uncompressed_buff_;
    uint32_t un_buff_size_;

    unsigned char* compressed_buff_;
    uint32_t c_buff_size_;
    void* wrkmem_;
};
}  // namespace toft
#endif  // TOFT_COMPRESS_BLOCK_LZO_H
