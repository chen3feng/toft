// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/base/string/format.h"
#include "toft/storage/sstable/sstable_reader.h"
#include "toft/storage/sstable/sstable_writer.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(SSTableReader, BadCases) {
    EXPECT_EQ(NULL,
              SSTableReader::Open("toft/storage/sstable/test/testdata/not_exist",
                                  SSTableReader::ON_DISK));
    EXPECT_EQ(NULL, SSTableReader::Open("toft/storage/sstable/test/testdata/empty",
                                        SSTableReader::ON_DISK));
}

TEST(SSTableReader, MetaData) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_metadata.sstable";
    option.set_path(path);
    SingleSSTableWriter builder(option);
    builder.AddOrDie("1", "2");
    builder.AddMetaData(kShardID, "2");
    builder.AddMetaData(kShardTotalNum, "5");
    builder.AddMetaData(kShardPolicy, "NA");
    builder.AddMetaData(kSSTableSetID, "121");
    EXPECT_TRUE(builder.Flush());

    toft::scoped_ptr<SSTableReader> sstable(SSTableReader::Open(path, SSTableReader::ON_DISK));
    EXPECT_TRUE(sstable.get());
    EXPECT_EQ("121", sstable->GetMetaData(kSSTableSetID));
    EXPECT_EQ("NA", sstable->GetMetaData(kShardPolicy));
    EXPECT_EQ("5", sstable->GetMetaData(kShardTotalNum));
    EXPECT_EQ("2", sstable->GetMetaData(kShardID));
}

}  // namespace toft
