// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/writer/single_sstable_writer.h"

#include <algorithm>

#include "toft/storage/file/file.h"
#include "toft/storage/sstable/sstable.h"

namespace toft {

SingleSSTableWriter::SingleSSTableWriter(const SSTableWriteOption &option)
                : SSTableWriter(option),
                  entry_count_(0),
                  total_bytes_(0),
                  index_offset_(0),
                  index_count_(0),
                  key_length_(0),
                  value_length_(0),
                  file_info_offset_(0),
                  flushed_(false) {
    block_.reset(new hfile::DataBlock(static_cast<CompressType>(option.compress_type())));
    index_.reset(new hfile::DataIndex);
    CHECK(!option_.path().empty());
}

SingleSSTableWriter::~SingleSSTableWriter() {
}

bool SingleSSTableWriter::Add(const std::string &key, const std::string &value) {
    d_data_.push_back(make_pair(key, value));
    return true;
}

void SingleSSTableWriter::AddMetaData(const std::string &key, const std::string &value) {
    file_info_meta_.insert(make_pair(key, value));
}

bool CompairString(std::deque<std::pair<std::string, std::string> >::iterator a,
                   std::deque<std::pair<std::string, std::string> >::iterator b) {
    return a->first < b->first;
}

bool SingleSSTableWriter::Flush() {
    CHECK(!flushed_) << "do not flush twice!";
    flushed_ = true;
    data_index_.reserve(d_data_.size());
    std::deque<std::pair<std::string, std::string> >::iterator it_d_data = d_data_.begin();
    for (; it_d_data != d_data_.end(); it_d_data++) {
        data_index_.push_back(it_d_data);
    }
    stable_sort(data_index_.begin(), data_index_.end(), CompairString);

    hfile::FileTrailer trailer;
    hfile::FileInfo fileInfo;
    std::map<std::string, std::string>::iterator it_fi_meta = file_info_meta_.begin();
    std::vector<std::deque<std::pair<std::string, std::string> >::iterator>::iterator it_d_index =
                    data_index_.begin();

    if (data_index_.empty()) {
        LOG(WARNING)<< "SingleSSTableWriter flush with no data, just ignore.";
        goto FAILED;
    }

    file_base_.reset(File::Open(GetTempSSTablePath(option_.path()), "w"));
    CHECK(file_base_.get()) << "open file error: "
                            << GetTempSSTablePath(option_.path());
    first_key_ = data_index_.front()->first;
    last_key_ = data_index_.back()->first;

    for (; it_d_index != data_index_.end(); it_d_index++) {
        size_t block_size = block_->GetUncompressedBufferSize();
        // write the block to disk
        if (block_size >= option_.block_size()) {
            if (!block_->WriteToFile(file_base_.get())) {
                LOG(ERROR)<< "fwrite error.";
                goto FAILED;
            }
            index_->AddDataBlockInfo(block_->GetCompressedBufferSize(),
                            block_->GetUncompressedBufferSize(), first_key_);
            total_bytes_ += block_size;
            first_key_ = (*it_d_index)->first;
            index_offset_ += block_->GetCompressedBufferSize();
            block_->ClearItems();
            ++index_count_;
        }
        key_length_ += (*it_d_index)->first.size();
        value_length_ += (*it_d_index)->second.size();
        block_->AddItem((*it_d_index)->first, (*it_d_index)->second);
        ++entry_count_;
    }

    // add file info meta
    for (; it_fi_meta != file_info_meta_.end(); ++it_fi_meta) {
        fileInfo.AddItem(it_fi_meta->first, it_fi_meta->second);
    }
    // write the last block
    if (!block_->WriteToFile(file_base_.get())) {
        LOG(ERROR)<< "fwrite error.";
        goto FAILED;
    }
    index_->AddDataBlockInfo(block_->GetCompressedBufferSize(), block_->GetUncompressedBufferSize(),
                             first_key_);
    total_bytes_ += block_->GetUncompressedBufferSize();
    index_offset_ += block_->GetCompressedBufferSize();
    ++index_count_;

    fileInfo.set_last_key(last_key_);
    if (entry_count_ != 0) {
        fileInfo.set_avg_key_len(key_length_ / entry_count_);
        fileInfo.set_avg_value_len(value_length_ / entry_count_);
    }

    file_info_offset_ = index_offset_;
    index_offset_ = index_offset_ + fileInfo.EncodeToString().length();

    if (!fileInfo.WriteToFile(file_base_.get())) {
        LOG(ERROR)<< "fwrite error.";
        goto FAILED;
    }

    if (!index_->WriteToFile(file_base_.get())) {
        LOG(ERROR)<< "fwrite error, size: " << index_->EncodeToString().size();
        goto FAILED;
    }
    trailer.set_file_info_offset(file_info_offset_);
    trailer.set_data_index_offset(index_offset_);
    trailer.set_data_index_count(index_count_);
    trailer.set_total_uncompressed_bytes(total_bytes_);
    trailer.set_entry_count(entry_count_);
    trailer.set_compress_type(option_.compress_type());
    if (!trailer.WriteToFile(file_base_.get())) {
        LOG(ERROR)<< "fwrite error.";
        goto FAILED;
    }
    file_base_->Flush();
    file_base_.reset(NULL);
    return MoveToRealPath(option_.path());

    FAILED: file_base_.reset(NULL);
    remove(GetTempSSTablePath(option_.path()).c_str());
    return false;
}

}  // namespace toft
