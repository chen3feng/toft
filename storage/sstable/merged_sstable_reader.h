// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_MERGED_SSTABLE_READER_H
#define TOFT_STORAGE_SSTABLE_MERGED_SSTABLE_READER_H

#include <string>
#include <vector>

#include "toft/base/scoped_ptr.h"
#include "toft/storage/sstable/sstable_reader.h"

#include "thirdparty/gflags/gflags.h"

DECLARE_bool(tolerate_sstable_open_failure);

//  GLOBAL_NOLINT(readability/casting)

namespace toft {

// NOTE: This class is NOT thread safe.
class MergedSSTableReader : public SSTableReader {
    TOFT_DECLARE_UNCOPYABLE(MergedSSTableReader);

public:
    MergedSSTableReader();
    ~MergedSSTableReader();

    // if param ignore_bad_files == true, the merged sstable will also
    // ignore bad file that miss meta data such as set id, shard id, etc.
    bool Open(const std::vector<std::string> &paths, ReadMode type, bool ignore_bad_files);

    bool Open(const std::vector<std::string> &paths);

    void GetPaths(std::vector<std::string> *paths) const;

    // Load one extra sstable.
    bool LoadSSTableReader(const std::string &path, ReadMode type);

public:
    virtual int EntryCount() const;

    // New a iterator to the key or the first one after the key if it's not found.
    // Caller owns the iterator.
    virtual Iterator *Seek(const std::string &key);

    // TODO(yeshunping): the single result of GetMetaData for merged sstable does not
    // make sense, we need another one.
    virtual const std::string GetMetaData(const std::string &key) const;
    void IterateMetaData(
                    toft::Closure<bool(const std::string &, const std::string &)> *callback) const;

    bool Lookup(const std::string &key, std::string *value);

private:
    friend class MergedIterator;
    friend class MergedReverseIterator;
    class Impl;
    toft::scoped_ptr<Impl> impl_;
};

}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_MERGED_SSTABLE_READER_H
