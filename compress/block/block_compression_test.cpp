// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include <string>

#include "toft/compress/block/block_compression.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

static const std::string& test_str = "asdgfsdglzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
static const std::string& test_empty_str = "";

void TestCompression(const std::string& name, const std::string& test_str) {
    BlockCompression* compression = TOFT_CREATE_BLOCK_COMPRESSION(name);
    EXPECT_TRUE(compression != NULL);
    std::string compressed_data;
    bool ret = compression->Compress(test_str.data(),
                                     test_str.size(),
                                     &compressed_data);
    EXPECT_TRUE(ret);
    LOG(INFO) << "raw len:" << test_str.length()
    << ", compressed len:" << compressed_data.size();

    std::string uncompressed_data;
    ret = compression->Uncompress(compressed_data.c_str(),
                    compressed_data.size(), &uncompressed_data);
    EXPECT_TRUE(ret);
    EXPECT_EQ(test_str, uncompressed_data);
    delete compression;
}

TEST(CompressionTest, SnappyCompression) {
    TestCompression("snappy", test_str);
    TestCompression("snappy", test_empty_str);
}

TEST(CompressionTest, LzoCompression) {
    TestCompression("lzo", test_str);
    TestCompression("lzo", test_empty_str);
}
}  // namespace toft
