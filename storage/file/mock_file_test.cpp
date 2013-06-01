// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/storage/file/mock_file.h"

#include "toft/base/scoped_ptr.h"

#include "thirdparty/gmock/gmock.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

using namespace testing;

TEST(MockFile, Open) {
    scoped_ptr<File> f1(File::Open("/mock/test", "r"));
    TOFT_FILE_EXPECT_CALL(*f1, Tell())
        .WillOnce(Return(1000))
        .WillRepeatedly(Return(-1));
    // On another file object with same path
    scoped_ptr<File> f2(File::Open("/mock/test", "r"));
    EXPECT_EQ(1000, f2->Tell());
    EXPECT_EQ(-1, f2->Tell());
}

TEST(MockFile, Unregister) {
    {
        scoped_ptr<File> f1(File::Open("/mock/test", "r"));
        TOFT_FILE_EXPECT_CALL(*f1, Tell())
            .WillOnce(Return(1000))
            .WillRepeatedly(Return(-1));
        EXPECT_EQ(1000, f1->Tell());
        EXPECT_EQ(-1, f1->Tell());
    }
    scoped_ptr<File> f2(File::Open("/mock/test", "r"));
    // The mock behavior set on f1 should not impact f2 because f1 has been
    // destructed.
    EXPECT_EQ(0, f2->Tell());
    TOFT_FILE_EXPECT_CALL(*f2, Tell()).WillRepeatedly(Return(1));
    EXPECT_EQ(1, f2->Tell());
}

TEST(MockFile, Read)
{
    scoped_ptr<File> file(File::Open("/mock/test", "r"));

    TOFT_FILE_EXPECT_CALL(*file, Read(_, _))
        .WillOnce(DoAll(FillArgBuffer<0>("hello", 5), Return(5)))
        .WillRepeatedly(Return(-1));

    char buffer[6] = {};
    EXPECT_EQ(5, file->Read(buffer, 5));
    EXPECT_STREQ("hello", buffer);
    EXPECT_EQ(-1, file->Read(buffer, 5));
}

TEST(MockFile, Write)
{
    scoped_ptr<File> file(File::Open("/mock/test", "w"));

    TOFT_FILE_EXPECT_CALL(*file, Write(_, _))
        .WillOnce(Return(5))
        .WillRepeatedly(Return(-1));

    EXPECT_EQ(5, file->Write("hello", 5));
    EXPECT_EQ(-1, file->Write("hello", 5));
}

TEST(MockFile, ReadLine)
{
    scoped_ptr<File> file(File::Open("/mock/test", "r"));

    TOFT_FILE_EXPECT_CALL(*file, ReadLine(_, _))
        .WillOnce(DoAll(SetArgPointee<0>("hello"), Return(true)))
        .WillRepeatedly(Return(false));

    std::string line;
    EXPECT_TRUE(file->ReadLine(&line));
    EXPECT_EQ("hello", line);
    EXPECT_FALSE(file->ReadLine(&line));
}

TEST(MockFile, Flush)
{
    scoped_ptr<File> file(File::Open("/mock/test", "w"));

    TOFT_FILE_EXPECT_CALL(*file, Flush())
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));

    EXPECT_TRUE(file->Flush());
    EXPECT_FALSE(file->Flush());
}

TEST(MockFile, Close)
{
    scoped_ptr<File> file(File::Open("/mock/test", "w"));

    TOFT_FILE_EXPECT_CALL(*file, Close())
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));

    EXPECT_TRUE(file->Close());
    EXPECT_FALSE(file->Close());
}

TEST(MockFile, Seek)
{
    scoped_ptr<File> file(File::Open("/mock/test", "w"));

    TOFT_FILE_EXPECT_CALL(*file, Seek(_, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));

    EXPECT_TRUE(file->Seek(SEEK_SET, 1000));
    EXPECT_FALSE(file->Seek(SEEK_CUR, 1000));
    EXPECT_FALSE(file->Seek(SEEK_END, -1000));
}

TEST(MockFile, Tell)
{
    scoped_ptr<File> file(File::Open("/mock/test", "w"));

    TOFT_FILE_EXPECT_CALL(*file, Tell())
        .WillOnce(Return(1000))
        .WillRepeatedly(Return(-1));

    EXPECT_EQ(1000, file->Tell());
    EXPECT_EQ(-1, file->Tell());
    EXPECT_EQ(-1, file->Tell());
}

/////////////////////////////////////////////////////////////////////////////
// Static mock tests.

TEST(MockFileSystem, Open)
{
    scoped_ptr<File> file(File::Open("/mock/test", "r"));
    EXPECT_FALSE(!file);
}

TEST(MockFileSystem, OpenFailed)
{
    FileSystemMock file_system_mock;
    EXPECT_CALL(file_system_mock, Open(_, _))
        .WillOnce(Return(static_cast<File*>(NULL)))
        .WillRepeatedly(Return(static_cast<File*>(NULL)));
    scoped_ptr<File> file(File::Open("/mock/test", "r"));
    EXPECT_TRUE(!file);
}

TEST(MockFileSystem, Rename)
{
    FileSystemMock file_system_mock;
    EXPECT_CALL(file_system_mock, Rename(_, _))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_TRUE(File::Rename("/mock/a", "/mock/b"));
    EXPECT_FALSE(File::Rename("/mock/a", "/mock/b"));
    EXPECT_FALSE(File::Rename("/mock/a", "/mock/b"));
}

TEST(MockFileSystem, Delete)
{
    FileSystemMock file_system_mock;
    EXPECT_CALL(file_system_mock, Delete(_))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_TRUE(File::Delete("/mock/test"));
    EXPECT_FALSE(File::Delete("/mock/test"));
    EXPECT_FALSE(File::Delete("/mock/test"));
}

TEST(MockFileSystem, Exists)
{
    FileSystemMock file_system_mock;
    EXPECT_CALL(file_system_mock, Exists(_))
        .WillOnce(Return(true))
        .WillRepeatedly(Return(false));
    EXPECT_TRUE(File::Exists("/mock/test"));
    EXPECT_FALSE(File::Exists("/mock/test"));
    EXPECT_FALSE(File::Exists("/mock/test"));
}

// Any mock method should die without a FileSystemMock object.
TEST(MockFileSystem, MockDeathTest)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";

    EXPECT_DEATH(File::Rename("/mock/a", "/mock/b"),
                 "FileSystemMock object must be defined");
    EXPECT_DEATH(File::Exists("/mock/test"), "FileSystemMock object must be defined");
}

TEST(MockFile, NestedStaticMock)
{
    FileSystemMock file_system_mock1;
    EXPECT_CALL(file_system_mock1, Rename(_, _)).WillRepeatedly(Return(true));
    EXPECT_TRUE(File::Rename("/mock/test", "/mock/test2"));

    {
        FileSystemMock file_system_mock2;
        EXPECT_CALL(file_system_mock2, Rename(_, _)).WillRepeatedly(Return(false));
        EXPECT_FALSE(File::Rename("/mock/test", "/mock/test2"));
    }

    EXPECT_TRUE(File::Rename("/mock/test", "/mock/test2"));
}

} // namespace toft
