// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/storage/file/file.h"
#include "toft/base/scoped_ptr.h"

namespace toft {

/////////////////////////////////////////////////////////////////////////////
// FileSystem

FileSystem::FileSystem() {}
FileSystem::~FileSystem() {}

FileSystem* File::GetFileSystemByPath(const std::string& file_path)
{
    // "/mfs/abc" -> "mfs"
    if (file_path[0] == '/') {
        size_t next_slash = file_path.find(1, '/');
        std::string prefix = file_path.substr(1, next_slash);
        FileSystem* fs = TOFT_GET_FILE_SYSTEM(prefix);
        if (fs != NULL)
            return fs;
    }
    return TOFT_GET_FILE_SYSTEM("local");
}

/////////////////////////////////////////////////////////////////////////////
// File

File::File() {}
File::~File() {}

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

bool File::ReadLines(const std::string& file_path, std::vector<std::string>* lines)
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

} // namespace toft

