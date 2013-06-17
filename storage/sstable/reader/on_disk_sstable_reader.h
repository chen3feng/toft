// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_READER_ON_DISK_SSTABLE_READER_H
#define TOFT_STORAGE_SSTABLE_READER_ON_DISK_SSTABLE_READER_H

#include <string>

#include "toft/base/scoped_ptr.h"
#include "toft/base/shared_ptr.h"
#include "toft/container/lru_cache.h"
#include "toft/storage/sstable/reader/sstable_reader_impl.h"
#include "toft/storage/sstable/sstable.h"

namespace toft {

// OnDiskSSTableReader is not good at key lookup but iteration.
class OnDiskSSTableReader : public SSTableReader {
    TOFT_DECLARE_UNCOPYABLE(OnDiskSSTableReader);

public:
    OnDiskSSTableReader();
    ~OnDiskSSTableReader();

    virtual Iterator *Seek(const std::string &key);

    std::shared_ptr<hfile::DataBlock> LoadDataBlock(int block_id);

    int GetBlockSize() const {
        return impl_->data_index_->GetBlockSize();
    }

    // For one key, find the minimal block that the key would probably in it.
    int FindMinimalBlock(const std::string &key) const {
        return impl_->data_index_->FindMinimalBlock(key);
    }

private:
    toft::scoped_ptr<LruCache<int, std::shared_ptr<hfile::DataBlock> > > block_cache_;
};

class OnDiskIterator : public SSTableReader::Iterator {
public:
    OnDiskIterator(OnDiskSSTableReader *sstable, const std::string &key);
    ~OnDiskIterator();

    virtual void Next();

    // return false if there's no item equal or larger than the query key
    void SeekKey(const std::string &key);

private:
    // let the info point to next data item
    // return false if it meets end of the table.
    bool NextItem();

    // get the key and value info of current item
    void LoadItem();

    OnDiskSSTableReader *sstable_;
    std::shared_ptr<hfile::DataBlock> cached_block_;
    int block_idx_;  // index of the current block
    int data_idx_;   // index of the current item in the block

TOFT_DECLARE_UNCOPYABLE(OnDiskIterator);
};

}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_READER_ON_DISK_SSTABLE_READER_H
