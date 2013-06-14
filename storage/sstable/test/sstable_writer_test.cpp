// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/base/closure.h"
#include "toft/base/stl_util.h"
#include "toft/base/string/format.h"
#include "toft/base/string/number.h"
#include "toft/storage/file/file.h"
#include "toft/storage/sstable/sstable.h"
#include "toft/storage/sstable/sstable_reader.h"
#include "toft/storage/sstable/sstable_writer.h"
#include "toft/storage/sstable/test/test_util.h"
#include "toft/storage/sstable/types.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

bool TestSmallMetaData(const std::string &key, const std::string &value) {
    if (key == "135") {
        return false;
    }
    LOG(INFO)<< "key:" << key << "; value:" << value;
    return true;
}

bool TestMetaData(const std::string &key, const std::string &value) {
    EXPECT_EQ(key + "_value", value);
    return true;
}

TEST(SingleSSTableWriter, BuildSingleBlockFile) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_one_block.sstable";
    option.set_path(path);
    SingleSSTableWriter builder(option);
    builder.AddOrDie("222", "111");
    builder.AddOrDie("111", "221");
    builder.AddMetaData("123", "456");
    EXPECT_TRUE(builder.Flush());

    toft::scoped_ptr<SSTableReader> sstable(SSTableReader::Open(path, SSTableReader::ON_DISK));
    EXPECT_TRUE(sstable.get());
    toft::scoped_ptr<SSTableReader::Iterator> iter(sstable->NewIterator());
    EXPECT_TRUE(iter->Valid());
    EXPECT_EQ("111", iter->key());
    EXPECT_EQ("221", iter->value());
    iter->Next();
    EXPECT_TRUE(iter->Valid());
    EXPECT_EQ("222", iter->key());
    EXPECT_EQ("111", iter->value());
    iter->Next();
    EXPECT_FALSE(iter->Valid());

    EXPECT_EQ("456", sstable->GetMetaData("123"));
}

TEST(SingleSSTableWriter, BuildMultiBlock) {
    SSTableWriteOption option;
    option.set_block_size(16);
    std::string path = "/tmp/test_multi_block.sstable";
    option.set_path(path);
    SingleSSTableWriter builder(option);
    builder.AddOrDie("222", "1111");
    builder.AddOrDie("111", "22222221");
    builder.AddMetaData("123", "456");
    builder.AddMetaData("135", "246");
    EXPECT_TRUE(builder.Flush());

    toft::scoped_ptr<SSTableReader> sstable(SSTableReader::Open(path, SSTableReader::ON_DISK));
    EXPECT_TRUE(sstable.get());
    toft::scoped_ptr<SSTableReader::Iterator> iter(sstable->NewIterator());
    EXPECT_TRUE(iter->Valid());
    EXPECT_EQ("111", iter->key());
    EXPECT_EQ("22222221", iter->value());
    iter->Next();
    EXPECT_TRUE(iter->Valid());
    EXPECT_EQ("222", iter->key());
    EXPECT_EQ("1111", iter->value());
    iter->Next();
    EXPECT_FALSE(iter->Valid());

    EXPECT_EQ("456", sstable->GetMetaData("123"));
    EXPECT_EQ("246", sstable->GetMetaData("135"));

    toft::Closure<bool(const std::string &, const std::string &)> *callback =  // NOLINT
                    toft::NewPermanentClosure(TestSmallMetaData);
    sstable->IterateMetaData(callback);
    delete callback;
}

void TestSSTableWriter(SSTableWriter *builder,
                        const std::string &sstable_path,
                        int test_number,
                        int key_mod, SSTableReader::ReadMode type) {
    LOG(INFO)<< "Start build sstable ...";
    for (int i = 0; i < test_number; ++i) {
        std::string key = GenKey(i, key_mod);
        std::string value = GenValue(i, key_mod);
        builder->AddOrDie(key, value);
        if (i > test_number/2 && i < test_number/2 + 10) {
            builder->AddMetaData(key, value);
        }
    }
    LOG(INFO) << "Start flush ...";
    EXPECT_TRUE(builder->Flush());
    LOG(INFO) << "Finish flush!";

    // make sure the sstable is right
    toft::scoped_ptr<SSTableReader> sstable(SSTableReader::Open(sstable_path, type));
    EXPECT_TRUE(sstable.get());
    toft::scoped_ptr<SSTableReader::Iterator> iter(sstable->NewIterator());
    LOG(INFO) << "start iteration";
    for (int i = 0; i < test_number; ++i) {
        std::string key = GenKey(i, key_mod);
        std::string value = GenValue(i, key_mod);
        iter->Next();
        if (i > test_number / 2 && i < test_number / 2 + 10) {
            EXPECT_EQ(key + "_value", sstable->GetMetaData(key)) << i;
        }
    }
    EXPECT_FALSE(iter->Valid());
    LOG(INFO) << "finish iteration";

    toft::Closure<bool (const std::string &, const std::string &)> *callback =
    toft::NewPermanentClosure(TestMetaData);
    sstable->IterateMetaData(callback);
    delete callback;
}

void TestSSTableWriterSeek(SSTableWriter *builder, const std::string &sstable_path,
                            int test_number, int key_mod, SSTableReader::ReadMode type) {
    LOG(INFO)<< "Start build sstable ...";
    for (int i = 0; i < test_number; ++i) {
        std::string key = GenKey(i, key_mod);
        std::string value = GenValue(i, key_mod);
        builder->AddOrDie(key, value);
        if (i > test_number/2 && i < test_number/2 + 10) {
            builder->AddMetaData(key, value);
        }
    }
    LOG(INFO) << "Start flush ...";
    EXPECT_TRUE(builder->Flush());
    LOG(INFO) << "Finish flush!";

    toft::scoped_ptr<SSTableReader> sstable(SSTableReader::Open(sstable_path, type));
    EXPECT_TRUE(sstable.get());
    for (int i = 0; i < test_number; ++i) {
        std::string key = GenKey(i, key_mod);
        std::string value = GenValue(i, key_mod);
        toft::scoped_ptr<SSTableReader::Iterator> iter(sstable->Seek(key));
        EXPECT_TRUE(iter->Valid()) << i;
        EXPECT_EQ(key, iter->key()) << i;
        EXPECT_EQ(value, iter->value()) << i;
        if (i > test_number/2 && i < test_number/2 + 10) {
            EXPECT_EQ(value, sstable->GetMetaData(key)) << i;
        }
    }

    toft::Closure<bool (const std::string &, const std::string &)> *callback =
    toft::NewPermanentClosure(TestMetaData);
    sstable->IterateMetaData(callback);
    delete callback;
}

TEST(SingleSSTableWriter, BuildLargeSingleFileOnDisk) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_single_large_disk.sstable";
    option.set_path(path);
    SingleSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::ON_DISK);
}

TEST(SingleSSTableWriter, BuildLargeSingleFileInMem) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_single_large_mem.sstable";
    option.set_path(path);
    SingleSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::IN_MEMORY);
}

TEST(SingleSSTableWriter, BuildSnappySingleFileOnDisk) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_single_snappy_disk.sstable";
    option.set_path(path);
    option.set_compress_type(CompressType_kSnappy);
    SingleSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::ON_DISK);
}

TEST(SingleSSTableWriter, BuildSnappySingleFileInMem) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_single_snappy_mem.sstable";
    option.set_path(path);
    option.set_compress_type(CompressType_kSnappy);
    SingleSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::IN_MEMORY);
}

TEST(SingleSSTableWriter, BuildLzoSingleFileOnDisk) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_single_lzo_disk.sstable";
    option.set_path(path);
    option.set_compress_type(CompressType_kLzo);
    SingleSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::ON_DISK);
}

TEST(SingleSSTableWriter, BuildLzoSingleFileInMem) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_single_lzo_mem.sstable";
    option.set_path(path);
    option.set_compress_type(CompressType_kLzo);
    SingleSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::IN_MEMORY);
}

TEST(UnsortedSSTableWriter, BuildLargeUnsortedFileOnDisk) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_unsorted_large_disk.sstable";
    option.set_path(path);
    UnsortedSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::ON_DISK);
}

TEST(UnsortedSSTableWriter, BuildLargeUnsortedFileInMem) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_unsorted_large_mem.sstable";
    option.set_path(path);
    UnsortedSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::IN_MEMORY);
}

TEST(UnsortedSSTableWriter, BuildSnappyUnsortedFileOnDisk) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_unsorted_snappy_disk.sstable";
    option.set_path(path);
    option.set_compress_type(CompressType_kSnappy);
    UnsortedSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::ON_DISK);
}

TEST(UnsortedSSTableWriter, BuildSnappyUnsortedFileInMem) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_unsorted_snappy_mem.sstable";
    option.set_path(path);
    option.set_compress_type(CompressType_kSnappy);
    UnsortedSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::IN_MEMORY);
}

TEST(UnsortedSSTableWriter, BuildLzoUnsortedFileOnDisk) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_unsorted_lzo_disk.sstable";
    option.set_path(path);
    option.set_block_size(64 * 1024 * 1024);
    option.set_compress_type(CompressType_kLzo);
    UnsortedSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::ON_DISK);
}

TEST(UnsortedSSTableWriter, BuildLzoUnsortedFileInMem) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_unsorted_lzo_mem.sstable";
    option.set_path(path);
    option.set_block_size(64 * 1024 * 1024);
    option.set_compress_type(CompressType_kLzo);
    UnsortedSSTableWriter builder(option);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::IN_MEMORY);
}

TEST(CompositedSSTableWriter, BuildLargeSingleFile) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_c_single_large_disk.sstable";
    option.set_path(path);
    CompositedSSTableWriter builder(option, 512 * 1024 * 1024);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::ON_DISK);
}

TEST(CompositedSSTableWriter, BuildLzoSingleFile) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_c_single_lzo_disk.sstable";
    option.set_path(path);
    option.set_compress_type(CompressType_kLzo);
    CompositedSSTableWriter builder(option, 512 * 1024 * 1024);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::ON_DISK);
}

TEST(CompositedSSTableWriter, BuildSnappySingleFile) {
    SSTableWriteOption option;
    std::string path = "/tmp/test_c_single_snappy_disk.sstable";
    option.set_path(path);
    option.set_compress_type(CompressType_kSnappy);
    CompositedSSTableWriter builder(option, 512 * 1024 * 1024);
    TestSSTableWriter(&builder, path, kTestNum, kMaxLength, SSTableReader::ON_DISK);
}

TEST(SSTableWriter, ReadStaticMeta) {
    std::string value;
    for (int i = 0; i < kTestNum; ++i) {
        std::string key = GenKey(i, kMaxLength);
        std::string expect_value = GenValue(i, kMaxLength);
        if (i > kTestNum / 2 && i < kTestNum / 2 + 10) {
            EXPECT_TRUE(SSTableReader::GetMetaData(
                            "/tmp/test_single_large_disk.sstable", key, &value));
            EXPECT_EQ(expect_value, value) << i;
        }
    }
}

TEST(ShardingSSTableWriterTest, Normal) {
    // Build
    SSTableWriteOption option;
    std::string prefix = "/tmp/test_sharding_meta";
    option.set_path(prefix);
    option.set_compress_type(CompressType_kSnappy);
    option.set_sharding_policy("FingerprintSharding");
    ShardingSSTableWriter builder(3, option);
    LOG(INFO)<< "Start build 3 sharding sstable ...";
    for (int i = 0; i < kTestNum; ++i) {
        std::string key = GenKey(i, kMaxLength);
        std::string value = GenValue(i, kMaxLength);
        builder.AddOrDie(key, value);
    }

    LOG(INFO)<< "Start flush ...";
    EXPECT_TRUE(builder.Flush());
    LOG(INFO)<< "Finish flush!";

    // Check meta data now
    std::string id;
    std::vector<SSTableReader*> sstables;
    for (int i = 0; i < 3; ++i) {
        std::string path = StringPrint("%s-%05d-of-%05d", prefix.c_str(), i, 3);
        ASSERT_TRUE(File::Exists(path));
        sstables.push_back(SSTableReader::Open(path, SSTableReader::ON_DISK));
        std::string value;
        ASSERT_TRUE(SSTableReader::GetMetaData(path, kShardID, &value));
        ASSERT_EQ(NumberToString(i), value);
        ASSERT_TRUE(SSTableReader::GetMetaData(path, kShardTotalNum, &value));
        ASSERT_EQ("3", value);
        ASSERT_TRUE(SSTableReader::GetMetaData(path, kShardPolicy, &value));
        ASSERT_EQ("FingerprintSharding", value);
        ASSERT_TRUE(SSTableReader::GetMetaData(path, kSSTableSetID, &value));
        if (id.empty()) {
            id = value;
            LOG(INFO)<< "sstable set id: " << id;
        } else {
            ASSERT_EQ(id, value);
        }
    }
    ASSERT_FALSE(File::Exists("/tmp/test_sharding_meta-00005-of-00005"));

    // Check data validty.
    for (int i = 0; i < kTestNum; ++i) {
        std::string key = GenKey(i, kMaxLength);
        std::string value = GenValue(i, kMaxLength);
        int count = 0;
        for (int j = 0; j < 3; ++j) {
            ASSERT_TRUE(sstables[j]);
            toft::scoped_ptr<SSTableReader::Iterator> iter(sstables[j]->Seek(key));
            if (iter->key() == key) {
                ++count;
                ASSERT_EQ(value, iter->value());
            }
        }
        ASSERT_EQ(1, count);
    }
    DeleteElements(&sstables);
}

}  // namespace toft
