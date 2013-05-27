// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/storage/file/file.h"

#include <errno.h>

#include "toft/base/scoped_ptr.h"

namespace toft {

/////////////////////////////////////////////////////////////////////////////
// FileSystem

FileSystem::FileSystem() {}
FileSystem::~FileSystem() {}

bool FileSystem::ReadAll(const std::string& file_path, std::string* buffer,
                         size_t max_size)
{
    scoped_ptr<File> fp(File::Open(file_path, "r"));
    if (!fp)
        return false;
    std::string tmp(max_size, '\0');
    int64_t nread = fp->Read(&tmp[0], max_size);
    if (nread < 0)
        return false;
    tmp.resize(nread);
    buffer->swap(tmp);
    return true;
}

bool FileSystem::ReadLines(const std::string& file_path,
                           std::vector<std::string>* lines)
{
    scoped_ptr<File> fp(File::Open(file_path, "r"));
    if (!fp)
        return false;
    lines->clear();
    std::string line;
    while (fp->ReadLine(&line))
        lines->push_back(line);
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// File

File::File() {}
File::~File() {}

FileSystem* File::GetFileSystemByPath(const std::string& file_path)
{
    // "/mfs/abc" -> "mfs"
    if (file_path[0] == '/') {
        size_t next_slash = file_path.find('/', 1);
        if (next_slash != std::string::npos) {
            std::string prefix = file_path.substr(1, next_slash - 1);
            FileSystem* fs = TOFT_GET_FILE_SYSTEM(prefix);
            if (fs != NULL)
                return fs;
        }
    }
    return TOFT_GET_FILE_SYSTEM("local");
}

File* File::Open(const std::string& file_path, const char* mode)
{
    FileSystem* fs = GetFileSystemByPath(file_path);
    return fs->Open(file_path, mode);
}

bool File::Exists(const std::string& file_path)
{
    FileSystem* fs = GetFileSystemByPath(file_path);
    return fs->Exists(file_path);
}

bool File::Delete(const std::string& file_path)
{
    FileSystem* fs = GetFileSystemByPath(file_path);
    return fs->Delete(file_path);
}

bool File::Rename(const std::string& from, const std::string& to) {
    FileSystem* from_fs = GetFileSystemByPath(from);
    FileSystem* to_fs = GetFileSystemByPath(to);
    if (from_fs != to_fs) {
        errno = EXDEV;
        return false;
    }
    return from_fs->Rename(from, to);
}

bool File::GetTimes(const std::string& file_path, FileTimes* times) {
    FileSystem* fs = GetFileSystemByPath(file_path);
    return fs->GetTimes(file_path, times);
}

bool File::ReadAll(const std::string& file_path, std::string* buffer,
                   size_t max_size)
{
    FileSystem* fs = GetFileSystemByPath(file_path);
    return fs->ReadAll(file_path, buffer, max_size);
}

bool File::ReadLines(const std::string& file_path, std::vector<std::string>* lines)
{
    FileSystem* fs = GetFileSystemByPath(file_path);
    return fs->ReadLines(file_path, lines);
}

FileIterator* File::Iterate(const std::string& dir, const std::string& pattern,
                            int include_types, int exclude_types) {
    FileSystem* fs = GetFileSystemByPath(dir);
    return fs->Iterate(dir, pattern, include_types, exclude_types);
}

} // namespace toft

