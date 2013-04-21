// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_STORAGE_FILE_FILE_H
#define TOFT_STORAGE_FILE_FILE_H
#pragma once

#include <stdint.h>
#include "toft/base/class_registry.h"
#include "toft/base/uncopyable.h"

namespace toft {

class FileSystem;

class File {
    DECLARE_UNCOPYABLE(File);
protected:
    File();
public:
    virtual ~File();
    virtual int64_t Read(void*     buffer,
                         int64_t   size) = 0;
    virtual int64_t Write(const void* buffer,
                          int64_t     size) = 0;
    virtual bool Flush() = 0;
    virtual bool Close() = 0;
    virtual bool Seek(int64_t offset, int whence) = 0;
    virtual int64_t Tell() = 0;
    virtual bool ReadLine(std::string* line, size_t max_size = 65536) = 0;

public:
    static File* Open(const std::string& file_path, const char* mode);
    static bool Exists(const std::string& file_path);
    static bool ReadLines(const std::string& file_path, std::vector<std::string>* lines);
private:
    static FileSystem* GetFileSystemByPath(const std::string& file_path);
};

class FileSystem {
    DECLARE_UNCOPYABLE(FileSystem);
protected:
    FileSystem();
    virtual ~FileSystem();
public:
    virtual File* Open(const std::string& file_path, const char* mode) = 0;
    virtual bool Exists(const std::string& file_path) = 0;
};

TOFT_CLASS_REGISTRY_DEFINE_SINGLETON(file_system, FileSystem);

#define TOFT_REGISTER_FILE_SYSTEM(prefix, class_name) \
    TOFT_CLASS_REGISTRY_REGISTER_CLASS_SINGLETON( \
        file_system, FileSystem, prefix, class_name)

#define TOFT_GET_FILE_SYSTEM(prefix) \
    TOFT_CLASS_REGISTRY_GET_SINGLETON(file_system, prefix)

#define TOFT_FILE_SYSTEM_COUNT() \
    TOFT_CLASS_REGISTRY_CLASS_COUNT(file_system)

#define TOFT_FILE_SYSTEM_NAME(i) \
    TOFT_CLASS_REGISTRY_CLASS_NAME(file_system, i)

} // namespace toft

#endif // TOFT_STORAGE_FILE_FILE_H
