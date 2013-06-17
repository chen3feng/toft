// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/base/string/format.h"
#include "toft/storage/sharding/fingerprint_sharding.h"
#include "toft/storage/sstable/merged_sstable_reader.h"
#include "toft/storage/sstable/sstable.h"
#include "toft/storage/sstable/sstable_reader.h"
#include "toft/storage/sstable/sstable_writer.h"
#include "toft/storage/sstable/test/test_util.h"
#include "toft/storage/sstable/types.h"

#include "thirdparty/glog/logging.h"

namespace toft {

bool TestMetaData(const std::string &key, const std::string &value) {
    EXPECT_EQ(key + "_value", value);
    return true;
}

TEST(MergedSSTableReader, ReadMultiFiles) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_single_lzo_disk.sstable";
    option.set_path(path);
    option.set_compress_type(CompressType_kSnappy);
    SingleSSTableWriter builder(option);

    SSTableWriteOption option1;
    std::string path1 = "/tmp/test_unsorted_lzo_disk.sstable";
    option1.set_path(path1);
    option1.set_compress_type(CompressType_kSnappy);
    UnsortedSSTableWriter builder1(option1);

    LOG(INFO)<< "Start build sstable :" << path;
    LOG(INFO)<< "Start build sstable :" << path1;
    for (int i = 0; i < kTestNum; ++i) {
        std::string key = GenKey(i, kMaxLength);
        std::string value = GenValue(i, kMaxLength);
        builder.Add(key, value);
        builder1.Add(key, value);
        if (i % 1000 == 0) {
            builder.AddMetaData(key, value);
            builder1.AddMetaData(key, value);
        }
    }
    LOG(INFO)<< "Start flush ...";
    EXPECT_TRUE(builder.Flush());
    EXPECT_TRUE(builder1.Flush());
    LOG(INFO)<< "Finish flush!";

    // open a sstable
    std::vector<std::string> paths;
    paths.push_back(path);
    paths.push_back(path1);

    MergedSSTableReader sstable;
    if (!sstable.Open(paths, SSTableReader::ON_DISK, true)) {
        LOG(INFO)<< "error open sstable!";
    }
    EXPECT_EQ(sstable.EntryCount(), 2 * kTestNum);
    toft::scoped_ptr<SSTableReader::Iterator> iter(sstable.NewIterator());
    LOG(INFO)<< "start iteration";
    for (int i = 0; i < kTestNum; ++i) {
        std::string key = GenKey(i, kMaxLength);
        std::string value = GenValue(i, kMaxLength);
        iter.reset(sstable.Seek(key));
        EXPECT_TRUE(iter->Valid()) << i;
        EXPECT_EQ(key, iter->key()) << i;
        EXPECT_EQ(key + "_value", iter->value()) << i;
        iter->Next();
        EXPECT_TRUE(iter->Valid()) << i;
        EXPECT_EQ(key, iter->key()) << i;
        EXPECT_EQ(key + "_value", iter->value()) << i;
        iter->Next();
    }
    ASSERT_FALSE(iter->Valid());
    LOG(INFO)<< "finish iteration";

    LOG(INFO)<< "start meta iteration";
    toft::Closure<bool(const std::string &, const std::string &)> *callback =  // NOLINT
                    toft::NewPermanentClosure(TestMetaData);
    sstable.IterateMetaData(callback);
    delete callback;
    LOG(INFO)<< "finish meta iteration";
}

TEST(MergedSSTableReader, Lookup) {
    SSTableWriteOption option;
    std::string prefix = "/tmp/test_merged_look_up";
    option.set_path(prefix);
    option.set_compress_type(CompressType_kSnappy);
    option.set_sharding_policy("FingerprintSharding");

    const int kNumShard = 5;
    FingerprintSharding sharding;
    sharding.SetShardingNumber(kNumShard);
    ShardingSSTableWriter builder(kNumShard, option);
    for (int i = 0; i < kTestNum; ++i) {
        std::string key = GenKey(i, kMaxLength);
        std::string value = GenValue(i, kMaxLength);
        builder.AddOrDie(key, value);
    }
    LOG(INFO)<< "before flush";
    builder.Flush();
    LOG(INFO)<< "after flush";

    MergedSSTableReader merged_sstable;
    std::vector<std::string> files;
    for (int i = 1; i < kNumShard; ++i) {
        std::string path = ShardingSSTableWriter::GetShardingPath(prefix, i, kNumShard);
        VLOG(2) << path;
        files.push_back(path);
    }

    LOG(INFO)<< "begin to open merged sstable!";
    ASSERT_TRUE(merged_sstable.Open(files, SSTableReader::ON_DISK, false));
    LOG(INFO)<< "open done!";

    for (int i = 0; i < kTestNum; ++i) {
        std::string key = GenKey(i, kMaxLength);
        std::string value = GenValue(i, kMaxLength);
        int idx = sharding.Shard(key);
        std::string value_result;
        if (idx != 0) {
            ASSERT_TRUE(merged_sstable.Lookup(key, &value_result))<< "key: " << key;
            ASSERT_EQ(value, value_result);
        } else {
            ASSERT_FALSE(merged_sstable.Lookup(key, &value_result));
        }
    }
    LOG(INFO)<< "done!";
}

}  // namespace toft
