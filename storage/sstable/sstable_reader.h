// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>
// C++ implementation of HBase's HFile. For the data format, please refer to
// http://hbase.apache.org/docs/current/api/org/apache/hadoop/hbase/io/hfile/HFile.html
//
// See sstable_test.cc for the usage.

#ifndef TOFT_STORAGE_SSTABLE_SSTABLE_READER_H
#define TOFT_STORAGE_SSTABLE_SSTABLE_READER_H

#include <stdio.h>
#include <string>
#include <vector>

#include "toft/base/closure.h"
#include "toft/base/scoped_ptr.h"
#include "toft/base/uncopyable.h"

// GLOBAL_NOLINT(readability/casting)

namespace toft {
namespace hfile {
class DataBlock;
class DataIndex;
class FileTrailer;
class FileInfo;
} // namespace hfile

class File;

// The file format is HFile 1.0. But the key and value are only std::string.
class SSTableReader {
    TOFT_DECLARE_UNCOPYABLE(SSTableReader);

public:
    class Iterator;
    class Impl;

    enum ReadMode {
        ON_DISK = 0,
        IN_MEMORY = 1
    };

    explicit SSTableReader(ReadMode type);
    virtual ~SSTableReader();


    // ON_DISK mode is not good at looking key
    // IN_MEMORY mode load data to memory, which is more efficient
    static SSTableReader *Open(const std::string &path, ReadMode type);
    static bool GetMetaData(const std::string &path, const std::string &key, std::string *value);
    static bool GetEntryCount(const std::string &path, int *count);

    virtual void Init() {
    }

    // return value of key, when can't find key, return empty string
    virtual const std::string GetMetaData(const std::string &key) const;
    virtual void IterateMetaData(
                    toft::Closure<bool(const std::string &, const std::string &)>* callback) const;

    virtual int EntryCount() const;
    virtual bool Lookup(const std::string &key, std::string *value);

    // New a iterator to the key, or the first one after the key if it's not found.
    // Caller should delete the iterator
    virtual Iterator* Seek(const std::string &key) = 0;
    // New a iterator pointed to the first key.
    // Caller should delete the iterator
    Iterator* NewIterator();

    std::string GetPath() const;

protected:
    SSTableReader();

    toft::scoped_ptr<SSTableReader::Impl> impl_;
};

class SSTableReader::Iterator {
    TOFT_DECLARE_UNCOPYABLE(Iterator);

public:
    Iterator();
    virtual ~Iterator();

    const std::string key() const;
    const std::string value() const;

    virtual void Next() = 0;
    //  If exist, seek to first position of this key.
    //  If not exist then
    //  1, If it is less than first key, seek to first key
    //  2, It's larger than last key, set Valid() to false.
    //  3, Seek iterator of nearest key
    virtual void SeekKey(const std::string &key) = 0;

    bool Valid() const;

protected:
    std::string key_;
    std::string value_;
    bool valid_;
};
}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_SSTABLE_READER_H
