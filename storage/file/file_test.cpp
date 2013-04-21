// Copyright 2010, The Toft Authors.
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/scoped_ptr.h"
#include "toft/storage/file/file.h"
#include "toft/storage/file/local_file.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

const char* const kFileName = "testfile.txt";

class FileTest : public testing::Test {
protected:
    void SetUp() {
    }
};

TEST_F(FileTest, Open) {
    scoped_ptr<File> fp(File::Open(kFileName, "r"));
    EXPECT_TRUE(fp);
}

TEST_F(FileTest, Close) {
}

TEST_F(FileTest, Read) {
    char buffer[5];
    scoped_ptr<File> fp(File::Open(kFileName, "r"));
    int64_t readn_len = fp->Read(buffer, sizeof(buffer));
    EXPECT_EQ(5, readn_len);
    EXPECT_EQ(0, memcmp(buffer, "hello", 5));
}

TEST_F(FileTest, Write) {
    scoped_ptr<File> fp(File::Open("file.dat", "w"));
    ASSERT_EQ(5, fp->Write("hello", 5));
}

TEST_F(FileTest, Seek) {
    scoped_ptr<File> fp(File::Open(kFileName, "r"));
    char buffer[5];
    ASSERT_TRUE(fp->Seek(5, SEEK_SET));
    int64_t readn_len = fp->Read(buffer, sizeof(buffer));
    EXPECT_EQ(5, readn_len);
    EXPECT_EQ(0, memcmp(buffer, "world", 5));
}

TEST_F(FileTest, Tell) {
    scoped_ptr<File> fp(File::Open(kFileName, "r"));
    ASSERT_EQ(0, fp->Tell());
    ASSERT_TRUE(fp->Seek(5, SEEK_SET));
    ASSERT_EQ(5, fp->Tell());
}

TEST_F(FileTest, ReadLine) {
    scoped_ptr<File> fp(File::Open(kFileName, "r"));
    std::string line;
    ASSERT_TRUE(fp->ReadLine(&line));
    EXPECT_EQ("helloworld1", line);
    ASSERT_TRUE(fp->ReadLine(&line));
    EXPECT_EQ("helloworld2", line);
    ASSERT_TRUE(fp->ReadLine(&line));
    EXPECT_EQ("helloworld3", line);
    ASSERT_TRUE(fp->ReadLine(&line));
    EXPECT_EQ("", line);
    ASSERT_FALSE(fp->ReadLine(&line));
}

TEST_F(FileTest, Exists) {
    EXPECT_TRUE(File::Exists(kFileName));
    EXPECT_FALSE(File::Exists("non-exist"));
}

TEST_F(FileTest, OpenDirAsFile)
{
    scoped_ptr<File> fp(File::Open("/bin", "wb"));
    ASSERT_FALSE(fp);
}

} // namespace toft

