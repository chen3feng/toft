// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_CRYPTO_HASH_CRC32_H
#define TOFT_CRYPTO_HASH_CRC32_H

#include <stdint.h>
#include <string>

#include "toft/base/string/string_piece.h"
#include "toft/base/uncopyable.h"

namespace toft {

class CRC32 {
    TOFT_DECLARE_UNCOPYABLE(CRC32);

public:
    CRC32();
    ~CRC32();

    //  Init is called in constructor, but if you want to use the same object
    //  for many times, you SHOULD call Init before computing sha1 of new data.
    void Init();
    void Update(StringPiece sp);
    uint32_t Final();
    // Finalizes the MD5 operation and fills the buffer with the digest.
    //  Data is uint32_t_t
    void Final(void* digest);
    //  Hex encoding for result
    std::string HexFinal();

    static uint32_t Digest(StringPiece sp);
    static std::string HexDigest(StringPiece sp);

private:
    void InitCrc32Table();

    uint32_t result_;
    uint32_t crc32_table_[256];
};

}  // namespace toft
#endif  // TOFT_CRYPTO_HASH_CRC32_H
