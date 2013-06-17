// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/base/closure.h"
#include "toft/base/string/format.h"
#include "toft/storage/sstable/sstable.h"
#include "toft/storage/sstable/sstable_reader.h"
#include "toft/storage/sstable/sstable_writer.h"
#include "toft/storage/sstable/types.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(CompositedSSTableReaderWriter, STableSort) {
    SSTableWriteOption option;
    std::string path = "/tmp/same_key_sorted_value.sstable";
    option.set_path(path);
    option.set_compress_type(CompressType_kSnappy);
    CompositedSSTableWriter builder(option, 128 * 1024);

    int test_number = 102400;
    LOG(INFO)<< "Start build sstable ...";
    //  std::string key = "same_key";
    for (int i = 0; i < test_number; ++i) {
        std::string key = StringPrint("key_%09d", i);
        std::string value = StringPrint("%09d", i);
        builder.AddOrDie(key, value);
    }
    LOG(INFO)<< "Start flush ...";
    EXPECT_TRUE(builder.Flush());
    LOG(INFO)<< "Finish flush!";

    toft::scoped_ptr<SSTableReader> sstable(SSTableReader::Open(path, SSTableReader::ON_DISK));
    ASSERT_TRUE(sstable.get());
    toft::scoped_ptr<SSTableReader::Iterator> iter(sstable->NewIterator());
    LOG(INFO)<< "start iteration";
    for (int i = 0; i < test_number; ++i) {
        std::string key = StringPrint("key_%09d", i);
        std::string value = StringPrint("%09d", i);
        EXPECT_EQ(key, iter->key()) << i;
        EXPECT_EQ(value, iter->value()) << i;
        iter->Next();
    }
    EXPECT_FALSE(iter->Valid());
    LOG(INFO)<< "finish iteration";
}
}  // namespace toft
