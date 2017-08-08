// Copyright 2013, Baidu Inc.
// Author: Liu Cheng <liucheng02@baidu.com>
//
#include "glog/logging.h"
#include "gtest/gtest.h"

#include "toft/base/array_size.h"
#include "toft/storage/file/local_file.h"
#include "toft/storage/seqfile/local_sequence_file_reader.h"

TEST(SequenceFileReaderTest, LoadAllRecordsAsString) {
    toft::LocalFileSystem local;
    toft::File* file = local.Open("testdata/seq_file", "rw");

    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileReader reader(file);

    std::string key, value;
    int count = 0;
    while (reader.ReadRecord(&key, &value)) {
        std::cout << "key:" << key << ", value:" << value << std::endl;
        ++count;
    }
    std::cout << "seq_file has " << count << " records" << std::endl;
    ASSERT_EQ(true, reader.is_finished());
    reader.Close();
}

TEST(SequenceFileReaderTest, LoadAllRecordAsStringPiece) {
    toft::LocalFileSystem local;
    toft::File* file = local.Open("testdata/seq_file", "rw");

    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileReader reader(file);

    toft::StringPiece key, value;
    while (reader.ReadRecord(&key, &value)) {
        std::cout << "key:" << std::string(key.data(), key.size())
            << ", value:" << std::string(value.data(), value.size()) << std::endl;
    }
    ASSERT_EQ(true, reader.is_finished());
    reader.Close();
}

TEST(SequenceFileReaderTest, SkipToSync) {
    toft::LocalFileSystem local;
    toft::File* file = local.Open("testdata/seq_file", "rw");

    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileReader reader(file);
    ASSERT_EQ(96, reader.SkipToSync(0));
    ASSERT_EQ(96, reader.SkipToSync(80));
    ASSERT_EQ(-1, reader.SkipToSync(81));

    ASSERT_EQ(96, reader.SkipToSync(80));
    toft::StringPiece key, value;
    while (reader.ReadRecord(&key, &value)) {
        std::cout << "key:" << std::string(key.data(), key.size())
            << ", value:" << std::string(value.data(), value.size()) << std::endl;
    }
    ASSERT_EQ(true, reader.is_finished());
    reader.Close();
}

TEST(SequenceFileReaderTest, Seek) {
    toft::LocalFileSystem local;
    toft::File* file = local.Open("testdata/seq_file", "rw");

    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileReader reader(file);
    ASSERT_EQ(true, reader.Seek(0));
    ASSERT_EQ(true, reader.Seek(80));
    ASSERT_EQ(false, reader.Seek(81));

    ASSERT_EQ(true, reader.Seek(80));
    toft::StringPiece key, value;
    while (reader.ReadRecord(&key, &value)) {
        std::cout << "key:" << std::string(key.data(), key.size())
            << ", value:" << std::string(value.data(), value.size()) << std::endl;
    }
    ASSERT_EQ(true, reader.is_finished());
    reader.Close();
}

TEST(SequenceFileReaderTest, Tell) {
    toft::LocalFileSystem local;
    toft::File* file = local.Open("testdata/seq_file", "rw");

    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileReader reader(file);
    ASSERT_EQ(true, reader.Seek(80));
    ASSERT_EQ(96, reader.Tell());

    toft::StringPiece key, value;
    for (int i = 0; i < 3; ++i) {
        while (reader.ReadRecord(&key, &value)) {
            std::cout << "key:" << std::string(key.data(), key.size())
                << ", value:" << std::string(value.data(), value.size()) << std::endl;
        }
        ASSERT_EQ(true, reader.is_finished());
        ASSERT_EQ(true, reader.Seek(0));
    }

    ASSERT_EQ(true, reader.is_finished());
    reader.Close();
}

TEST(SequenceFileReaderTest, ReadBigFile) {
    toft::LocalFileSystem local;
    toft::File* file = local.Open("testdata/big_seq_file", "rw");

    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileReader reader(file);
    ASSERT_EQ(true, reader.Seek(80));
    ASSERT_EQ(96, reader.Tell());

    toft::StringPiece key, value;
    int count = 0;
    while (reader.ReadRecord(&key, &value)) {
        ++count;
    }
    std::cout << "big_seq_file record num:" << count << std::endl;

    int n = 0;
    unsigned seed = getpid();
    for (int i = 0; i < 100; ++i) {
        while (reader.ReadRecord(&key, &value)) {
            ++n;
        }
        n = 0;
        ASSERT_EQ(true, reader.is_finished());
        ASSERT_EQ(true, reader.Seek(rand_r(&seed) % (3 * 1024 * 1024)));
    }

    ASSERT_EQ(true, reader.is_finished());
    reader.Close();
}

void CheckReadNRecord(toft::LocalSequenceFileReader* reader, int n) {
    int count = 0;
    toft::StringPiece key, value;
    while (reader->ReadRecord(&key, &value)) {
        ++count;
    }
    if (n < 0) {
        n = 0;
    }
    if (n > 11282) {  // the max number of records in big_seq_file
        n = 11282;
    }
    EXPECT_EQ(n, count);
}

TEST(SequenceFileReaderTest, seek_to_tail_test) {
    toft::LocalFileSystem local;
    toft::File* file = local.Open("testdata/big_seq_file", "rw");

    ASSERT_NE(static_cast<toft::File*>(NULL), file);

    toft::LocalSequenceFileReader reader(file);
    ASSERT_TRUE(reader.Init());
    ASSERT_EQ(true, reader.Seek(80));
    ASSERT_EQ(96, reader.Tell());

    int tests[] = { -1, 0, 1, 2, 5, 10, 100, 1000,
                    2 * 1000, 3 * 1000, 5 * 1000, 10 * 1000, 100 * 1000, 200 * 1000, 300 * 1000,
                    0, 1, 2, 5 };
    unsigned seed = getpid();
    for (size_t i = 0; i < TOFT_ARRAY_SIZE(tests); ++i) {
        reader.Seek(rand_r(&seed) % (3 * 1024 * 1024));
        EXPECT_TRUE(reader.SeekToTail(tests[i]));
        CheckReadNRecord(&reader, tests[i]);
    }
}
