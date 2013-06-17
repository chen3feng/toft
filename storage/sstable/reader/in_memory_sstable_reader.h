// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_READER_IN_MEMORY_SSTABLE_READER_H
#define TOFT_STORAGE_SSTABLE_READER_IN_MEMORY_SSTABLE_READER_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "toft/base/scoped_ptr.h"
#include "toft/storage/sstable/sstable_reader.h"

namespace toft {
namespace hfile {
class DataBlock;
} // namespace hfile

typedef std::vector<std::pair<std::string, std::vector<std::string> > > DataVector;

class InMemorySSTableReader : public SSTableReader {
    TOFT_DECLARE_UNCOPYABLE(InMemorySSTableReader);
public:
    InMemorySSTableReader();
    ~InMemorySSTableReader();

    virtual void Init();

    virtual Iterator *Seek(const std::string &key);

private:
    friend class InMemoryIterator;

    toft::scoped_ptr<hfile::DataBlock> cached_block_;
    //  Replace with hash_map
    std::map<std::string, DataVector::iterator> index_;
    DataVector data_;
};

class InMemoryIterator : public SSTableReader::Iterator {
    TOFT_DECLARE_UNCOPYABLE(InMemoryIterator);

public:
    InMemoryIterator(InMemorySSTableReader *sstable, const std::string &key);
    ~InMemoryIterator();

    void SeekKey(const std::string &key);
    virtual void Next();

private:
    bool NextItem();
    void LoadItem();

    InMemorySSTableReader *sstable_;
    DataVector::iterator cur_it_;
    int pos_;
    int size_;
};

}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_READER_IN_MEMORY_SSTABLE_READER_H
