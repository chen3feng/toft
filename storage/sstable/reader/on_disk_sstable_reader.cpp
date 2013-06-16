// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/reader/on_disk_sstable_reader.h"

#include <algorithm>

#include "thirdparty/gflags/gflags.h"

DEFINE_int32(on_disk_sstable_block_cache, 128,
             "max # of item in the block cache for one on disk sstable");

namespace toft {

OnDiskSSTableReader::OnDiskSSTableReader()
                : block_cache_(new LruCache<int, std::shared_ptr<hfile::DataBlock> >(
                               FLAGS_on_disk_sstable_block_cache)) {
}

OnDiskSSTableReader::~OnDiskSSTableReader() {
}

std::shared_ptr<hfile::DataBlock> OnDiskSSTableReader::LoadDataBlock(int block_id) {
    std::shared_ptr<hfile::DataBlock> block;
    if (!block_cache_->Get(block_id, &block)) {
        // not in cache,
        hfile::DataBlock *new_block = new hfile::DataBlock(impl_->file_trailer_->compress_type());
        if (!impl_->LoadDataBlock(block_id, new_block)) {
            delete new_block;
            LOG(ERROR)<< "fail to load data block!";
            return std::shared_ptr<hfile::DataBlock>();
        }
        block.reset(new_block);
        block_cache_->Put(block_id, block);
    }
    return block;
}

SSTableReader::Iterator *OnDiskSSTableReader::Seek(const std::string &key) {
    return new OnDiskIterator(this, key);
}

OnDiskIterator::OnDiskIterator(OnDiskSSTableReader *sstable, const std::string &key)
                : sstable_(sstable),
                  block_idx_(-1),
                  data_idx_(-1) {
    SeekKey(key);
    if (valid_) {
        LoadItem();
    }
}

OnDiskIterator::~OnDiskIterator() {
}

void OnDiskIterator::Next() {
    NextItem();
    if (valid_)
        LoadItem();
}

void OnDiskIterator::SeekKey(const std::string &key) {
    if (key == "") {
        LOG(INFO)<< "block size:" << sstable_->GetBlockSize();
    }
    valid_ = false;
    // the sstable is empty.
    if (sstable_->GetBlockSize() == 0)
    return;

    int tmp_block_idx = sstable_->FindMinimalBlock(key);
    if (block_idx_ != tmp_block_idx) {
        block_idx_ = tmp_block_idx;
        cached_block_ = sstable_->LoadDataBlock(block_idx_);
        if (!cached_block_.get())
        return;
    }

    data_idx_ = 0;
    key_ = cached_block_->GetKey(data_idx_);
    if (key_ < key) {  // check if the first item is what we need
        while (NextItem()) {
            key_ = cached_block_->GetKey(data_idx_);
            if (key_ >= key) break;
        }
        if (!valid_) return;
    } else {
        valid_ = true;
    }
    value_ = cached_block_->GetValue(data_idx_);
}

bool OnDiskIterator::NextItem() {
    valid_ = false;
    // reaches the last item in the block
    if (data_idx_ == cached_block_->GetDataItemSize() - 1) {
        // reaches the last block, no more data
        if (block_idx_ == sstable_->GetBlockSize() - 1) {
            return false;
        }
        ++block_idx_;
        data_idx_ = 0;
        cached_block_ = sstable_->LoadDataBlock(block_idx_);
        if (!cached_block_.get())
            return false;
    } else {
        ++data_idx_;
    }
    valid_ = true;
    return true;
}

void OnDiskIterator::LoadItem() {
    key_ = cached_block_->GetKey(data_idx_);
    value_ = cached_block_->GetValue(data_idx_);
}

}  // namespace toft
