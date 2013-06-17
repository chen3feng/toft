// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_HASH_CRC32_H
#define TOFT_HASH_CRC32_H

#include <stdint.h>
#include <string>

#include "toft/base/string/string_piece.h"
#include "toft/base/uncopyable.h"

namespace toft {

class CRC32 {
public:
    CRC32();
    ~CRC32();

    //  Init is called in constructor, but if you want to use the same object
    //  for many times, you SHOULD call Init before computing sha1 of new data.
    void Init();
    void Update(StringPiece sp);
    uint32_t Final() const;
    // Finalizes the CRC operation and fills the buffer with the digest.
    //  Data is uint32_t_t
    void Final(void* digest) const;
    //  Hex encoding for result
    std::string HexFinal() const;

    static uint32_t Digest(StringPiece sp);
    static std::string HexDigest(StringPiece sp);

private:
    uint32_t result_;
};

}  // namespace toft

#endif  // TOFT_HASH_CRC32_H
