// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_STORAGE_FILE_FILE_H
#define TOFT_STORAGE_FILE_FILE_H
#pragma once

#include <stdint.h>
#include <time.h>
#include <string>
#include <vector>
#include "toft/base/class_registry.h"
#include "toft/base/uncopyable.h"

namespace toft {

class FileSystem;

struct FileTimes {
    time_t access_time;
    time_t modify_time;
    time_t change_time;
};

// File type flags
enum FileType {
    FileType_None = 0,      // Nothing
    FileType_Regular = 1,   // Regular file
    FileType_Directory = 2, // Directory
    FileType_Link = 4,      // Symbolic Link
    FileType_All = FileType_Regular | FileType_Directory | FileType_Link,
};

// File iteration entry.
struct FileEntry {
    int type; // FileType enum combination.
    std::string name; // Without dir.
};

// To iterate file entries in a dir.
class FileIterator {
protected:
    FileIterator() {}
public:
    virtual ~FileIterator() {}

    // Return true if next file entry exist and obtained.
    // Return false means iteration is complete.
    virtual bool GetNext(FileEntry* entry) = 0;
};

// A abstruct object.
//
// All errors are reported by errno.
class File {
    TOFT_DECLARE_UNCOPYABLE(File);

protected:
    // You can't construct a File object, you must carete it by the static Open
    // method.
    File();

public:
    virtual ~File();

    // Read at most size bytes into buffer.
    // Return the number of bytes read.
    // Return -1 if error occurs.
    virtual int64_t Read(void* buffer, int64_t size) = 0;

    // Read at most size bytes into buffer.
    // Return the number of bytes read.
    // Return -1 if error occurs.
    virtual int64_t Write(const void* buffer, int64_t size) = 0;

    // Write of all user-space buffered data to file system.
    virtual bool Flush() = 0;

    // Close a file object. After closed, all other operations are invalid.
    // You can call close multiple time safely. Close a closed file object
    // will return true.
    virtual bool Close() = 0;

    // Seek to offset from whence.
    virtual bool Seek(int64_t offset, int whence) = 0;

    // Return current offset of file pointer from start.
    virtual int64_t Tell() = 0;

    // Read next text line into *line, end of line will be stripped.
    // Read at most max_size if no eol found.
    virtual bool ReadLine(std::string* line, size_t max_size = 65536) = 0;

public:
    // The returned File* object is created by new and can be deleted.
    // So it can be stored into a scoped_ptr.
    static File* Open(const std::string& file_path, const char* mode);

    // Check whether a path exists.
    static bool Exists(const std::string& file_path);

    // Delete file
    static bool Delete(const std::string& file_path);

    // Rename file
    static bool Rename(const std::string& from, const std::string& to);

    // Get times attributes of path
    static bool GetTimes(const std::string& file_path, FileTimes* times);

    // Read all bytes into *buffer, at most max_size if file too large.
    static bool ReadAll(const std::string& file_path, std::string* buffer,
                        size_t max_size = 64*1024*1024);

    // Read all lines into *lines. You must ensure the file is not to large
    // to fit into memory.
    static bool ReadLines(const std::string& file_path,
                          std::vector<std::string>* lines);

    // Get a iterator to iterate the entries of the dir.
    static FileIterator* Iterate(const std::string& dir,
                                 const std::string& pattern = "*",
                                 int include_type = FileType_All,
                                 int exclude_type = FileType_None);

private:
    static FileSystem* GetFileSystemByPath(const std::string& file_path);
};

// A abstruct FileSystem object represent a why of file processing.
// FileSystem must be registered as singleton.
class FileSystem {
    TOFT_DECLARE_UNCOPYABLE(FileSystem);
protected:
    FileSystem();
    virtual ~FileSystem();
public:
    virtual File* Open(const std::string& file_path, const char* mode) = 0;
    virtual bool Exists(const std::string& file_path) = 0;
    virtual bool Delete(const std::string& file_path) = 0;
    virtual bool Rename(const std::string& from, const std::string& to) = 0;
    virtual bool GetTimes(const std::string& file_path, FileTimes* times) = 0;
    virtual bool ReadAll(const std::string& file_path, std::string* buffer,
                         size_t max_size);
    virtual bool ReadLines(const std::string& file_path,
                           std::vector<std::string>* lines);
    virtual FileIterator* Iterate(const std::string& dir,
                                  const std::string& pattern,
                                  int include_types,
                                  int exclude_types) = 0;
};

// Defile the file_system class registry, user can register their own
// file_system class singleton.
TOFT_CLASS_REGISTRY_DEFINE_SINGLETON(file_system, FileSystem);

} // namespace toft

// Register used defined file_system
// prefix is the prefix of path between the first and second '/'.
// the class_name should be a subclass of FileSystem
#define TOFT_REGISTER_FILE_SYSTEM(prefix, class_name) \
    TOFT_CLASS_REGISTRY_REGISTER_CLASS_SINGLETON( \
        file_system, FileSystem, prefix, class_name)

// Get FileSystem singleton from prefix.
#define TOFT_GET_FILE_SYSTEM(prefix) \
    TOFT_CLASS_REGISTRY_GET_SINGLETON(file_system, prefix)

// Count of registed file systems.
#define TOFT_FILE_SYSTEM_COUNT() TOFT_CLASS_REGISTRY_CLASS_COUNT(file_system)

// Get file systems name by index
#define TOFT_FILE_SYSTEM_NAME(i) TOFT_CLASS_REGISTRY_CLASS_NAME(file_system, i)

#endif // TOFT_STORAGE_FILE_FILE_H
