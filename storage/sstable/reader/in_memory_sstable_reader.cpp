// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/reader/in_memory_sstable_reader.h"
#include "toft/storage/sstable/reader/on_disk_sstable_reader.h"
#include "toft/storage/sstable/reader/sstable_reader_impl.h"

namespace toft {

InMemorySSTableReader::InMemorySSTableReader() {
}

InMemorySSTableReader::~InMemorySSTableReader() {
}

SSTableReader::Iterator *InMemorySSTableReader::Seek(const std::string &key) {
    return new InMemoryIterator(this, key);
}

void InMemorySSTableReader::Init() {
    cached_block_.reset(new hfile::DataBlock(impl_->file_trailer_->compress_type()));
    std::vector<std::string> values;
    std::string ori_key = "";
    for (int block_id = 0; block_id < impl_->file_trailer_->data_index_count(); block_id++) {
        impl_->LoadDataBlock(block_id, cached_block_.get());
        for (int data_idx = 0; data_idx < cached_block_->GetDataItemSize(); data_idx++) {
            if (block_id == 0 && data_idx == 0) {
                ori_key = cached_block_->GetKey(0);
            }
            std::string key = cached_block_->GetKey(data_idx);
            std::string value = cached_block_->GetValue(data_idx);
            if (key != ori_key) {
                data_.push_back(make_pair(ori_key, values));
                ori_key = key;
                values.clear();
            }
            values.push_back(value);
        }
    }
    data_.push_back(make_pair(ori_key, values));
//  index_.resize(data_.size() * 2);
    for (DataVector::iterator it = data_.begin(); it != data_.end(); it++) {
        index_.insert(make_pair(it->first, it));
    }
}

InMemoryIterator::InMemoryIterator(InMemorySSTableReader *sstable, const std::string &key)
                : sstable_(sstable) {
    SeekKey(key);
    if (valid_) {
        LoadItem();
    }
}

InMemoryIterator::~InMemoryIterator() {
}

void InMemoryIterator::Next() {
    NextItem();
    if (valid_) {
        LoadItem();
    }
}

bool InMemoryIterator::NextItem() {
    valid_ = false;
    if (pos_ < size_ - 1) {
        pos_++;
    } else {
        cur_it_++;
        if (cur_it_ != sstable_->data_.end()) {
            pos_ = 0;
            size_ = cur_it_->second.size();
        } else {
            return false;
        }
    }
    valid_ = true;
    return true;
}

void InMemoryIterator::LoadItem() {
    key_ = cur_it_->first;
    value_ = cur_it_->second.at(pos_);
}

void InMemoryIterator::SeekKey(const std::string &key) {
    bool found = false;
    std::map<std::string, DataVector::iterator>::iterator it = sstable_->index_.find(key);
    if (it != sstable_->index_.end()) {
        cur_it_ = it->second;
        found = true;
    }
    // FIXME(yesp) : is it right?
    if (!found && (key < sstable_->data_.begin()->first)) {
        cur_it_ = sstable_->data_.begin();
        found = true;
    }
    if (!found && (key > sstable_->data_.back().first)) {
        valid_ = false;
        return;
    }
    if (!found) {
        DataVector::size_type n = sstable_->data_.size();
        unsigned int begin = 0;
        unsigned int end = n - 1;
        unsigned int mid = 0;
        while (begin <= end) {
            mid = (begin + end) / 2;
            LOG(INFO)<< "begin:" << begin << ";mid:" << mid << ";end:" << end;
            const std::pair<std::string, std::vector<std::string> >& midPair = sstable_->data_[mid];
            if (midPair.first == key) {
                break;
            } else if (midPair.first < key) {
                begin = mid + 1;
            } else if (midPair.first > key) {
                end = mid - 1;
            }
        }
        cur_it_ = sstable_->data_.begin() + mid;
    }
    pos_ = 0;
    size_ = cur_it_->second.size();
    valid_ = true;
    return;
}

}  // namespace toft
