// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/storage/file/mock_file.h"
#include "thirdparty/glog/logging.h"

namespace toft {

FileMock::FileMock(MockFileSystem* fs, const std::string& path,
                   const std::shared_ptr<MockFile>& file)
    : m_fs(fs), m_path(path), m_mock(file) {}

FileMock::~FileMock() {
    m_mock.reset();
    m_fs->Unregister(m_path);
}

int64_t FileMock::Read(void* buffer, int64_t size) {
    return m_mock->Read(buffer, size);
}

int64_t FileMock::Write(const void* buffer, int64_t size) {
    return m_mock->Write(buffer, size);
}

bool FileMock::Flush() {
    return m_mock->Flush();
}

bool FileMock::Close() {
    return m_mock->Close();
}

bool FileMock::Seek(int64_t offset, int whence) {
    return m_mock->Seek(offset, whence);
}

int64_t FileMock::Tell() {
    return m_mock->Tell();
}

bool FileMock::ReadLine(std::string* line, size_t max_size) {
    return m_mock->ReadLine(line, max_size);
}

MockFile* FileMock::FromFile(File* file, const char* filename, int line)
{
    FileMock* mock = dynamic_cast<FileMock*>(file); // NOLINT(runtime/rtti)
    CHECK(mock != NULL) << filename << ":" << line
        << ": First param of EXPECT_FILE_CALL is not a Mock File object";
    return mock->m_mock.get();
}

const MockFile* FileMock::FromFile(const File* file, const char* filename, int line)
{
    return FromFile(const_cast<File*>(file), filename, line);
}

FileSystemMock::FileSystemMock() : m_old(MockFileSystem::s_mock) {
    MockFileSystem::s_mock = this;
}

FileSystemMock::~FileSystemMock() {
    MockFileSystem::s_mock = m_old;
}

FileSystemMock* MockFileSystem::s_mock;

#define CHECK_FILE_SYSTEM_MOCK() \
    CHECK(s_mock != NULL) \
        << "FileSystemMock object must be defined to test this method: " \
        << __PRETTY_FUNCTION__

void MockFileSystem::Unregister(const std::string& path) {
    OpenedFileMap::iterator i = m_opened_files.find(path);
    if (i != m_opened_files.end()) {
        // Remove entry if no any opening file object.
        if (i->second.unique())
            m_opened_files.erase(i);
    }
}

File* MockFileSystem::Open(const std::string& file_path, const char* mode) {
    if (s_mock)
        return s_mock->Open(file_path, mode);
    OpenedFileMap::iterator i = m_opened_files.find(file_path);
    if (i == m_opened_files.end()) {
        std::shared_ptr<MockFile> file(new MockFile);
        i = m_opened_files.insert(std::make_pair(file_path, file)).first;
    }
    return new FileMock(this, file_path, i->second);
}

bool MockFileSystem::Exists(const std::string& file_path) {
    CHECK_FILE_SYSTEM_MOCK();
    return s_mock->Exists(file_path);
}

bool MockFileSystem::Delete(const std::string& file_path) {
    CHECK_FILE_SYSTEM_MOCK();
    return s_mock->Delete(file_path);
}

bool MockFileSystem::Rename(const std::string& from, const std::string& to) {
    CHECK_FILE_SYSTEM_MOCK();
    return s_mock->Rename(from, to);
}

bool MockFileSystem::GetTimes(const std::string& file_path, FileTimes* times) {
    CHECK_FILE_SYSTEM_MOCK();
    return s_mock->GetTimes(file_path, times);
}

bool MockFileSystem::ReadAll(const std::string& file_path, std::string* buffer,
                             size_t max_size) {
    CHECK_FILE_SYSTEM_MOCK();
    return s_mock->ReadAll(file_path, buffer, max_size);
}

bool MockFileSystem::ReadLines(const std::string& file_path,
                               std::vector<std::string>* lines) {
    CHECK_FILE_SYSTEM_MOCK();
    return s_mock->ReadLines(file_path, lines);
}

FileIterator* MockFileSystem::Iterate(const std::string& dir,
                                      const std::string& pattern,
                                      int include_types,
                                      int exclude_types) {
    CHECK_FILE_SYSTEM_MOCK();
    return s_mock->Iterate(dir, pattern, include_types, exclude_types);
}

TOFT_REGISTER_FILE_SYSTEM("mock", MockFileSystem);

} // namespace toft


