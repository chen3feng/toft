// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include <string>

#include "toft/hash/hash.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {
TEST(HashUnittest, Fingerprint) {
    const std::string& url = "http://app.kid.qq.com/exam/5528/5528_103392.htm";
    uint64_t hash_value = Fingerprint64(url);
    EXPECT_EQ(hash_value, 17105673616436300159UL);
    std::string str = Fingerprint64ToString(hash_value);
    EXPECT_EQ(str, "7F09753F868F63ED");
    uint64_t hash2 = StringToFingerprint64(str);
    EXPECT_EQ(hash_value, hash2);
}
}  // namespace toft
