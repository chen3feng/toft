// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/hash/crc32.h"

#include "toft/base/array_size.h"
#include "toft/encoding/hex.h"

// This implementation is based on the sample implementation in RFC 1952.

namespace {
// CRC32 polynomial, in reversed form.
// See RFC 1952, or http://en.wikipedia.org/wiki/Cyclic_redundancy_check
static const uint32_t kCrc32Polynomial = 0xEDB88320;
}

namespace toft {
CRC32::CRC32() {
    InitCrc32Table();
}

CRC32::~CRC32() {}

void CRC32::InitCrc32Table() {
    Init();
    //  Init crc32_table_
    for (uint32_t i = 0; i < TOFT_ARRAY_SIZE(crc32_table_); ++i) {
        uint32_t c = i;
        for (size_t j = 0; j < 8; ++j) {
            if (c & 1) {
                c = kCrc32Polynomial ^ (c >> 1);
            } else {
                c >>= 1;
            }
        }
        crc32_table_[i] = c;
    }
}

void CRC32::Init() {
    result_ = 0U;
}

void CRC32::Update(StringPiece sp) {
    uint32_t c = result_ ^ 0xFFFFFFFF;
    const uint8_t* u = reinterpret_cast<const uint8_t*>(sp.data());
    for (size_t i = 0; i < sp.size(); ++i) {
        c = crc32_table_[(c ^ u[i]) & 0xFF] ^ (c >> 8);
    }
    result_ = c ^ 0xFFFFFFFF;
}

uint32_t CRC32::Final() {
    return result_;
}

void CRC32::Final(void* data) {
    memcpy(data, &result_, 4);
}

std::string CRC32::HexFinal() {
    uint8_t digest[4];
    Final(&digest);
    return Hex::EncodeAsString(digest, 4);
}

uint32_t CRC32::Digest(StringPiece sp) {
    CRC32 crc32;
    crc32.Update(sp);
    return crc32.Final();
}

std::string CRC32::HexDigest(StringPiece sp) {
    CRC32 crc32;
    crc32.Update(sp);
    return crc32.HexFinal();
}

}  // namespace toft
