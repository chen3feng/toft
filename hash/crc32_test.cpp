// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/hash/crc32.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(Crc32Test, TestBasic) {
    EXPECT_EQ(0U, CRC32::Digest(""));
    EXPECT_EQ(0x352441C2U, CRC32::Digest("abc"));
    EXPECT_EQ(0x171A3F5FU,
              CRC32::Digest("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"));
}

TEST(Crc32Test, TestMultipleUpdates) {
    std::string input = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    CRC32 crc32;
    for (size_t i = 0; i < input.size(); ++i) {
        crc32.Update(StringPiece(input.data() + i, 1));
    }
    EXPECT_EQ(0x171A3F5FU, crc32.Final());
}

} // namespace toft
