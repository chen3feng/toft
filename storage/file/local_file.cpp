// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include "toft/storage/file/local_file.h"

#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "toft/base/string/algorithm.h"
#include "toft/base/unique_ptr.h"
#include "thirdparty/glog/logging.h"

namespace toft {

/////////////////////////////////////////////////////////////////////////////
// LocalFileSystem

File* LocalFileSystem::Open(const std::string& file_path, const char* mode)
{
    unique_ptr<FILE, int (*)(FILE*)> fp(fopen(file_path.c_str(), mode), &fclose);
    if (!fp)
        return NULL;
    return new LocalFile(fp.release());
}

bool LocalFileSystem::Exists(const std::string& file_path)
{
    return access(file_path.c_str(), F_OK) == 0;
}

bool LocalFileSystem::IsDir(const std::string& dir) {
  struct stat buf;

  if (lstat(dir.c_str(), &buf) < 0) {
    LOG(ERROR) << "lstat error for dir:" << dir;
    return false;
  }

  return S_ISDIR(buf.st_mode);
}

bool LocalFileSystem::Delete(const std::string& file_path)
{
    return remove(file_path.c_str()) == 0;
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
    int64_t nread = fread(buffer, 1, size, m_fp);
    if (nread == 0 && size > 0 && ferror(m_fp))
        return -1;
    return nread;
}

int64_t LocalFile::Write(const void* buffer,
                         int64_t size)
{
    int64_t nwrite = fwrite(buffer, 1, size, m_fp);
    if (nwrite == 0 && size > 0 && ferror(m_fp))
        return -1;
    return nwrite;
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
    return fseeko(m_fp, offset, whence) == 0;
}

int64_t LocalFile::Tell()
{
    return ftello(m_fp);
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
