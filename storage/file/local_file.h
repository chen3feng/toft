// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_STORAGE_FILE_LOCAL_FILE_H
#define TOFT_STORAGE_FILE_LOCAL_FILE_H
#pragma once

#include <stdio.h>
#include <string>
#include "toft/base/scoped_array.h"
#include "toft/base/string/string_piece.h"
#include "toft/storage/file/file.h"

namespace toft {

class LocalFileSystem : public FileSystem {
public:
    virtual File* Open(const std::string& file_path, const char* mode);
    virtual FileType GetFileType(const std::string &file_path);
    virtual bool Exists(const std::string& file_path);
    virtual bool Delete(const std::string& file_path);
    virtual bool Rename(const std::string& from, const std::string& to);
    virtual bool GetTimes(const std::string& file_path, FileTimes* times);
    virtual FileIterator* Iterate(const std::string& dir,
                                  const std::string& pattern,
                                  int include_types,
                                  int exclude_types);
    virtual bool Mkdir(const std::string& path, int mode);
};

// Represent a file object on local mounted file system
class LocalFile : public File {
    friend class LocalFileSystem;
    explicit LocalFile(FILE* fp, const std::string& file_path, const char* mode);
public:
    virtual ~LocalFile();

    // Implement File interface.
    //
    virtual int64_t Read(void* buffer, int64_t size);
    virtual int64_t Write(const void* buffer, int64_t size);
    virtual bool Flush();
    virtual bool Close();
    virtual bool Seek(int64_t offset, int whence);
    virtual int64_t Tell();
    virtual bool ReadLine(std::string* line, size_t max_size);
    virtual bool ReadLine(StringPiece* line, size_t max_size);
    virtual bool ReadLineWithLineEnding(std::string* line, size_t max_size);
    virtual bool ReadLineWithLineEnding(StringPiece* line, size_t max_size);

    virtual bool IsEof();

private:
    bool BufferedReadLine(size_t max_size, size_t* bytes_read);
    void ResizeBuffer(size_t buffer_size);

private:
    FILE* m_fp;
    scoped_array<char> m_buffer;
    size_t m_buffer_size;
};

} // namespace toft

#endif // TOFT_STORAGE_FILE_LOCAL_FILE_H
