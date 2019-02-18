// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_STORAGE_FILE_MOCK_FILE_H
#define TOFT_STORAGE_FILE_MOCK_FILE_H
#pragma once

#include <string.h>
#include <map>
#include <string>
#include <vector>

#include "toft/base/shared_ptr.h"
#include "toft/base/uncopyable.h"
#include "toft/storage/file/file.h"

#include "thirdparty/gmock/gmock.h"

// GLOBAL_NOLINT(whitespace/parens)

namespace toft {

class MockFileSystem;

// Implemantation detail: The actual mock File object.
class MockFile : public File
{
public:
    MOCK_METHOD2(Read, int64_t (void* buffer, int64_t size));
    MOCK_METHOD2(Write, int64_t (const void* buffer, int64_t size));
    MOCK_METHOD0(Flush, bool ());
    MOCK_METHOD0(Close, bool ());
    MOCK_METHOD2(Seek, bool (int64_t offset, int whence));
    MOCK_METHOD0(Tell, int64_t ());
    MOCK_METHOD2(ReadLine, bool (std::string* line, size_t max_size));
};

// File object returned by File::Open, wrapper a real MockFile object, and
// forward any method call to the wrapped MockFile object.
// Introduce this class make File::Open on same path return different File
// object but share the same MockFile object. So we can open a mock file
// and set it's behavior to impact the FileMock object opened by the tested
// function.
class FileMock : public File {
public:
    FileMock(MockFileSystem* fs, const std::string& path,
             const std::shared_ptr<MockFile>& file);
    virtual ~FileMock();
    virtual int64_t Read(void* buffer, int64_t size);
    virtual int64_t Write(const void* buffer, int64_t size);
    virtual bool Flush();
    virtual bool Close();
    virtual bool Seek(int64_t offset, int whence);
    virtual int64_t Tell();
    virtual bool ReadLine(std::string* line, size_t max_size);

    // Get the wrapped MockFile object from FileMock object.
    // NOTE: To implement EXPECT_FILE_CALL only.
    static const MockFile* FromFile(const File* file, const char* filename, int line);
    static MockFile* FromFile(File* file, const char* filename, int line);

private:
    MockFileSystem* m_fs;
    std::string m_path;
    std::shared_ptr<MockFile> m_mock;
};

// Mock object to mock static method of file.
//
// Example:
//
// TEST(MockFile, Rename)
// {
//     FileSystemMock file_static_mock;
//     EXPECT_CALL(file_system_mock, Rename(_, _))
//         .WillOnce(Return(false));
//     EXPECT_FALSE(File::Move("/mock/a", "/mock/b"));
// }
//
// In the duration of file_system_mock object, all static method of File class
// are mocked by this object, you are able to custom the behavior by gmock.
//
class FileSystemMock : public FileSystem {
    TOFT_DECLARE_UNCOPYABLE(FileSystemMock);
public:
    FileSystemMock();
    ~FileSystemMock();
    MOCK_METHOD2(Open, File* (const std::string& file_path, const char* mode));
    MOCK_METHOD1(Exists, bool (const std::string& file_path));
    MOCK_METHOD1(Delete, bool (const std::string& file_path));
    MOCK_METHOD2(Rename, bool (const std::string& from, const std::string& to));
    MOCK_METHOD2(GetTimes, bool (const std::string& file_path, FileTimes* times));
    MOCK_METHOD3(ReadAll, bool (const std::string& file_path, std::string* buffer,
                                size_t max_size));
    MOCK_METHOD2(ReadLines, bool (const std::string& file_path,
                                  std::vector<std::string>* lines));
    MOCK_METHOD4(Iterate, FileIterator* (const std::string& dir,
                                         const std::string& pattern,
                                         int include_types,
                                         int exclude_types));
private:
    FileSystemMock* m_old;
};

class MockFileSystem : public FileSystem {
    friend class FileSystemMock;
    friend class FileMock;
private:
    void Unregister(const std::string& path);
    virtual File* Open(const std::string& file_path, const char* mode);
    virtual bool Exists(const std::string& file_path);
    virtual bool Delete(const std::string& file_path);
    virtual bool Rename(const std::string& from, const std::string& to);
    virtual bool GetTimes(const std::string& file_path, FileTimes* times);
    virtual bool ReadAll(const std::string& file_path, std::string* buffer,
                         size_t max_size);
    virtual bool ReadLines(const std::string& file_path,
                           std::vector<std::string>* lines);
    virtual FileIterator* Iterate(const std::string& dir,
                                  const std::string& pattern,
                                  int include_types,
                                  int exclude_types);
private:
    static FileSystemMock* s_mock;
    // We expect call File::Open with same path returns same MockFile object.
    // So we can set mock behavior out of called functions.
    typedef std::map<std::string, std::shared_ptr<MockFile> > OpenedFileMap;
    OpenedFileMap m_opened_files;
};

// Action FillArgBuffer<k>(data, length) copies the elements in
// source range [data, data+length) to the buffer pointed to by the k-th
// (0-based) argument.
ACTION_TEMPLATE(FillArgBuffer,
                HAS_1_TEMPLATE_PARAMS(int, k),
                AND_2_VALUE_PARAMS(data, length)) {
    memcpy(::std::get<k>(args), data, length);
}

} // namespace toft

// Similar to EXPECT_CALL, but the first param file must be a mock file object.
#define TOFT_FILE_EXPECT_CALL(file, call) \
    EXPECT_CALL(*::toft::FileMock::FromFile(&(file), __FILE__, __LINE__), \
                call)

#define TOFT_FILE_ON_CALL(file, call) \
    ON_CALL(*::toft::FileMock::FromFile(&(file), __FILE__, __LINE__), \
            call)

#endif // TOFT_STORAGE_FILE_MOCK_FILE_H
