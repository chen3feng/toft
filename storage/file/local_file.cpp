// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/storage/file/local_file.h"
#include <string.h>
#include <unistd.h>
#include "toft/base/string/algorithm.h"

namespace toft {

/////////////////////////////////////////////////////////////////////////////
// LocalFileSystem

File* LocalFileSystem::Open(const std::string& file_path, const char* mode)
{
    FILE* fp = fopen(file_path.c_str(), mode);
    if (!fp)
        return NULL;
    try {
        return new LocalFile(fp);
    } catch (std::bad_alloc) {
        fclose(fp);
    }
    return NULL;
}

bool LocalFileSystem::Exists(const std::string& file_path)
{
    return access(file_path.c_str(), F_OK) == 0;
}

TOFT_REGISTER_FILE_SYSTEM("local", LocalFileSystem);

/////////////////////////////////////////////////////////////////////////////
// LocalFile

LocalFile::LocalFile(FILE* fp) : m_fp(fp)
{
}

LocalFile::~LocalFile()
{
    Close();
}

int64_t LocalFile::Read(void* buffer, int64_t size)
{
    size_t nread = fread(buffer, 1, size, m_fp);
    if (nread > 0 || size == 0)
        return nread;
    if (ferror(m_fp))
        return -1;
    return nread;
}

int64_t LocalFile::Write(const void* buffer,
                         int64_t size)
{
    return fwrite(buffer, 1, size, m_fp);
}

bool LocalFile::Flush()
{
    return fflush(m_fp) == 0;
}

bool LocalFile::Close()
{
    if (m_fp == NULL)
        return true;
    FILE* fp = m_fp;
    m_fp = NULL;
    return fclose(fp) == 0;
}

bool LocalFile::Seek(int64_t offset, int whence)
{
    return fseek(m_fp, offset, whence) == 0;
}

int64_t LocalFile::Tell()
{
    return ftell(m_fp);
}

bool LocalFile::ReadLine(std::string* line, size_t max_size)
{
    line->resize(max_size + 1);
    char* p = fgets(&(*line)[0], max_size, m_fp);
    if (!p)
        return false;
    line->resize(strlen(p));
    RemoveLineEnding(line);
    return true;
}

} // namespace toft
