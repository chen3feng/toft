// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/compress/block/lzo.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/lzo/lzo1x.h"

namespace {
const int kInitBuffSize = 64 * 1024;
}

namespace toft {

LzoCompression::LzoCompression()
                : uncompressed_buff_(NULL),
                  un_buff_size_(kInitBuffSize),
                  c_buff_size_(kInitBuffSize) {
    //  initialize the LZO library
    CHECK(lzo_init() == LZO_E_OK) << "internal error - lzo_init() failed !!!";
    uncompressed_buff_ = new unsigned char[kInitBuffSize];
    compressed_buff_ = new unsigned char[kInitBuffSize];
    //  Allocate blocks and the work-memory
    wrkmem_ = (lzo_voidp) malloc(LZO1X_1_MEM_COMPRESS);
}

LzoCompression::~LzoCompression() {
    delete uncompressed_buff_;
    delete compressed_buff_;
    free(wrkmem_);
}

bool LzoCompression::DoCompress(const char* str, size_t length, std::string* out) {
    size_t outsize = (length + length / 16 + 64 + 3);
    if (c_buff_size_ < outsize) {
        delete compressed_buff_;
        c_buff_size_ *= 2;
        compressed_buff_ = new unsigned char[c_buff_size_];
        VLOG(8) << "malloc larger space :" << c_buff_size_;
        CHECK(compressed_buff_) << "fail to new space";
    }

    lzo_uint out_len = c_buff_size_;
    int r = lzo1x_1_compress(reinterpret_cast<const unsigned char*>(str), length,
                             compressed_buff_,
                             &out_len, wrkmem_);

    if (r == LZO_E_OK) {
        VLOG(8) << "compressed  " << length << "  bytes into  " << out_len << " bytes";
    } else {
        LOG(ERROR)<< "internal error - compression failed";
        return false;
    }

    out->assign(reinterpret_cast<const char*>(compressed_buff_), out_len);
    return true;
}

bool LzoCompression::DoUncompress(const char* str, size_t length, std::string* out) {
try_again_with_a_bigger_buffer:

    lzo_uint out_len = un_buff_size_;
    int lzo_ret = lzo1x_decompress(reinterpret_cast<const unsigned char*>(str), length,
                                   uncompressed_buff_, &out_len, NULL);
    if (lzo_ret == LZO_E_OK) {
        VLOG(8) << "in:" << length << ", out:" << out_len;
        out->assign(reinterpret_cast<const char*>(uncompressed_buff_), out_len);
        return true;
    } else if (lzo_ret == LZO_E_OUTPUT_OVERRUN) {
        delete uncompressed_buff_;
        if (un_buff_size_ >= max_unCompressed_size_) {
            return false;
        }
        un_buff_size_ *= 2;
        VLOG(8) << "LZO_E_OUTPUT_OVERRUN, trying again with " << un_buff_size_
                << "byte buffer";
        goto try_again_with_a_bigger_buffer;
    } else {
        LOG(ERROR)<< "fail to Uncompress data!";
        return false;
    }

    return false;
}
TOFT_REGISTER_BLOCK_COMPRESSION(LzoCompression, "lzo");
}  // namespace toft
