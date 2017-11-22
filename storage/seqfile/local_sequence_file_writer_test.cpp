// Copyright 2013, Baidu Inc. All rights reserved.
// Author: Liu Cheng <liucheng02@baidu.com>
//
#include "glog/logging.h"
#include "gtest/gtest.h"

#include "toft/base/scoped_array.h"
#include "toft/base/string/number.h"
#include "toft/storage/file/local_file.h"
#include "toft/storage/seqfile/local_sequence_file_reader.h"
#include "toft/storage/seqfile/local_sequence_file_writer.h"

int VerifyFile(const std::string& file_name) {
    toft::LocalFileSystem local;
    toft::File* file = local.Open(file_name, "rw");
    toft::LocalSequenceFileReader reader(file);
    std::string key, value;
    int i = 0;
    while (reader.ReadRecord(&key, &value)) {
        ++i;
    }
    return i;
}

TEST(LocalSequenceFileWriter, write_no_record) {
    toft::LocalFileSystem local;
    const char* write_no_record = "write_no_record";
    toft::File* file = local.Open(write_no_record, "w");
    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileWriter writer(file);
    ASSERT_TRUE(writer.Init());
    writer.Close();

    ASSERT_EQ(0, VerifyFile(write_no_record));
}

TEST(LocalSequenceFileWriter, write_1_record) {
    toft::LocalFileSystem local;
    const char* write_1_record = "write_1_record";
    toft::File* file = local.Open(write_1_record, "w");
    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileWriter writer(file);
    ASSERT_TRUE(writer.Init());

    std::string hello = "hello";
    std::string world = "world";
    ASSERT_TRUE(writer.WriteRecord(hello, world));
    writer.Close();
    ASSERT_EQ(1, VerifyFile(write_1_record));
}

TEST(LocalSequenceFileWriter, write_101_record) {
    toft::LocalFileSystem local;
    const char* write_101_record = "write_101_record";
    toft::File* file = local.Open(write_101_record, "w");
    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileWriter writer(file);
    ASSERT_TRUE(writer.Init());

    for (int i = 0; i < 101; ++i) {
        std::string value = "world" + toft::NumberToString(i);
        ASSERT_TRUE(writer.WriteRecord("hello", value));
    }
    writer.Close();
    ASSERT_EQ(101, VerifyFile(write_101_record));
}

TEST(LocalSequenceFileWriter, write_many_record) {
    toft::LocalFileSystem local;
    const char* write_many_record = "write_many_record";
    toft::File* file = local.Open(write_many_record, "w");
    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileWriter writer(file);
    ASSERT_TRUE(writer.Init());

    int many = 10 * 1000;
    for (int i = 0; i < many; ++i) {
        std::string value = "world" + toft::NumberToString(i);
        ASSERT_TRUE(writer.WriteRecord("hello", value));
    }
    writer.Close();
    ASSERT_EQ(many, VerifyFile(write_many_record));
}

TEST(LocalSequenceFileWriter, write_large_record) {
    toft::LocalFileSystem local;
    const char* write_large_record = "write_large_record";
    toft::File* file = local.Open(write_large_record, "w");
    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileWriter writer(file);
    ASSERT_TRUE(writer.Init());

    int many = 1000;
    unsigned seed = getpid();
    toft::scoped_array<char> buffer(new char[4096]);

    for (int i = 0; i < many; ++i) {
        int r = rand_r(&seed) % 4096;
        std::string value(buffer.get(), r);
        ASSERT_TRUE(writer.WriteRecord("hello", value));
    }
    writer.Close();
    ASSERT_EQ(many, VerifyFile(write_large_record));
}

