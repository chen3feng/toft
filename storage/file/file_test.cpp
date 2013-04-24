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

TEST_F(FileTest, ReadError) {
    char buffer[1];
    scoped_ptr<File> fp(File::Open("/dev/null", "r"));
    int64_t nread = fp->Read(buffer, sizeof(buffer));
    EXPECT_EQ(0, nread);
}

TEST_F(FileTest, Write) {
    scoped_ptr<File> fp(File::Open("file.dat", "w"));
    ASSERT_EQ(5, fp->Write("hello", 5));
}

TEST_F(FileTest, WriteError) {
    scoped_ptr<File> fp(File::Open("/dev/full", "w"));
    static char data[32*1024]; // data must be large enough to avoid buffered.
    int64_t nwrite = fp->Write(data, sizeof(data));
    EXPECT_EQ(-1, nwrite);
}

TEST_F(FileTest, Flush) {
    scoped_ptr<File> fp(File::Open("file.dat", "w"));
    int64_t nwrite = fp->Write("hello", 5);
    EXPECT_EQ(5, nwrite);
    EXPECT_TRUE(fp->Flush());
}

TEST_F(FileTest, FlushError) {
    scoped_ptr<File> fp(File::Open("/dev/full", "w"));
    int64_t nwrite = fp->Write("hello", 5);
    EXPECT_EQ(5, nwrite); // Buffered
    EXPECT_FALSE(fp->Flush());
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

TEST_F(FileTest, ReadAll) {
    std::string data;
    EXPECT_FALSE(File::ReadAll("non-exist.dat", &data));
    EXPECT_TRUE(File::ReadAll(kFileName, &data));
    EXPECT_EQ("helloworld1\nhelloworld2\nhelloworld3\n\n", data);
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

TEST_F(FileTest, ReadLines) {
    std::vector<std::string> lines;
    ASSERT_TRUE(File::ReadLines(kFileName, &lines));
    EXPECT_EQ(4U, lines.size());
    EXPECT_EQ("helloworld1", lines[0]);
    EXPECT_EQ("helloworld2", lines[1]);
    EXPECT_EQ("helloworld3", lines[2]);
    EXPECT_EQ("", lines[3]);

}

TEST_F(FileTest, Exists) {
    EXPECT_TRUE(File::Exists(kFileName));
    EXPECT_FALSE(File::Exists("non-exist"));
}

TEST_F(FileTest, Delete) {
    {
        scoped_ptr<File> fp(File::Open("file.dat", "w"));
    }
    EXPECT_TRUE(File::Delete("file.dat"));
    EXPECT_FALSE(File::Delete("no-file.dat"));
}

TEST_F(FileTest, OpenDirAsFile)
{
    scoped_ptr<File> fp(File::Open("/bin", "wb"));
    ASSERT_FALSE(fp);
}

} // namespace toft

