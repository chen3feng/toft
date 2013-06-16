// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/hfile/coding.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {
namespace hfile {

TEST(Coding, Fixed32) {
    std::string s;
    for (uint32_t v = 0; v < 100000; v++) {
        PutFixed32(&s, v);
    }

    const char* p = s.data();
    for (uint32_t v = 0; v < 100000; v++) {
        uint32_t actual = DecodeFixed32(p);
        ASSERT_EQ(v, actual);
        p += sizeof(actual);
    }
}

TEST(Coding, Fixed64) {
    std::string s;
    for (int power = 0; power <= 63; power++) {
        uint64_t v = static_cast<uint64_t>(1) << power;
        PutFixed64(&s, v - 1);
        PutFixed64(&s, v + 0);
        PutFixed64(&s, v + 1);
    }

    const char* p = s.data();
    for (int power = 0; power <= 63; power++) {
        uint64_t v = static_cast<uint64_t>(1) << power;
        uint64_t actual;
        actual = DecodeFixed64(p);
        ASSERT_EQ(v-1, actual);
        p += sizeof(actual);

        actual = DecodeFixed64(p);
        ASSERT_EQ(v+0, actual);
        p += sizeof(actual);

        actual = DecodeFixed64(p);
        ASSERT_EQ(v+1, actual);
        p += sizeof(actual);
    }
}

// Test that encoding routines generate little-endian encodings
TEST(Coding, EncodingOutput) {
    std::string dst;
    PutFixed32(&dst, 0x04030201);
    ASSERT_EQ(4UL, dst.size());
    ASSERT_EQ(0x01, static_cast<int>(dst[0]));
    ASSERT_EQ(0x02, static_cast<int>(dst[1]));
    ASSERT_EQ(0x03, static_cast<int>(dst[2]));
    ASSERT_EQ(0x04, static_cast<int>(dst[3]));

    dst.clear();
    PutFixed64(&dst, 0x0807060504030201ull);
    ASSERT_EQ(8UL, dst.size());
    ASSERT_EQ(0x01, static_cast<int>(dst[0]));
    ASSERT_EQ(0x02, static_cast<int>(dst[1]));
    ASSERT_EQ(0x03, static_cast<int>(dst[2]));
    ASSERT_EQ(0x04, static_cast<int>(dst[3]));
    ASSERT_EQ(0x05, static_cast<int>(dst[4]));
    ASSERT_EQ(0x06, static_cast<int>(dst[5]));
    ASSERT_EQ(0x07, static_cast<int>(dst[6]));
    ASSERT_EQ(0x08, static_cast<int>(dst[7]));
}

}  // namespace hfile
}  // namespace toft
