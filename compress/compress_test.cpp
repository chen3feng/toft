// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include <string>

#include "toft/compress/compress.h"

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

namespace toft {

static const std::string& test_str = "asdgfsdglzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";

void TestCompression(const std::string& name) {
  Compression* compression = CREATE_COMPRESSION(name);
  EXPECT_TRUE(compression != NULL);
  std::string compressed_data;
  bool ret = compression->Compress(test_str.c_str(),
      test_str.size(), &compressed_data);
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
  TestCompression("SnappyCompression");
}
}  // namespace toft
