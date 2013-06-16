// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/writer/unsorted_sstable_writer.h"

#include "toft/storage/file/file.h"
#include "toft/storage/sstable/sstable.h"

#include "thirdparty/gflags/gflags.h"

namespace toft {

UnsortedSSTableWriter::UnsortedSSTableWriter(const SSTableWriteOption &option)
                : SSTableWriter(option),
                  failed_(false),
                  is_first_key_(true),
                  entry_count_(0),
                  total_bytes_(0),
                  index_offset_(0),
                  index_count_(0),
                  key_length_(0),
                  value_length_(0),
                  file_info_offset_(0) {
    block_.reset(new hfile::DataBlock(static_cast<CompressType>(option.compress_type())));
    index_.reset(new hfile::DataIndex);
    CHECK(!option_.path().empty());
    std::string path = GetTempSSTablePath(option_.path());
    file_base_.reset(File::Open(path, "w"));
    CHECK(file_base_.get()) << "open file error: " << option_.path();
}

UnsortedSSTableWriter::~UnsortedSSTableWriter() {
}

bool UnsortedSSTableWriter::Add(const std::string &key, const std::string &value) {
    if (failed_) {
        return false;
    }

    if (is_first_key_) {
        is_first_key_ = false;
        first_key_ = key;
    }

    if (block_->GetUncompressedBufferSize() >= static_cast<int64_t>(option_.block_size())) {
        if (!WriteBlockAndUpdateIndex()) {
            failed_ = true;
            return false;
        }
        first_key_ = key;
    }
    block_->AddItem(key, value);
    key_length_ += key.length();
    value_length_ += value.length();
    last_key_ = key;
    ++entry_count_;
    return true;
}

void UnsortedSSTableWriter::AddMetaData(const std::string &key, const std::string &value) {
    file_info_meta_.insert(make_pair(key, value));
}

bool UnsortedSSTableWriter::Flush() {
    if (failed_)
        return false;
    CHECK(file_base_.get()) << "don't call Flush twice!";

    // Write the last block
    if (!WriteBlockAndUpdateIndex())
        return false;

    hfile::FileInfo fileInfo;
    std::map<std::string, std::string>::iterator it_fi_meta = file_info_meta_.begin();
    // Add file info meta
    for (; it_fi_meta != file_info_meta_.end(); ++it_fi_meta) {
        fileInfo.AddItem(it_fi_meta->first, it_fi_meta->second);
    }
    fileInfo.set_last_key(last_key_);
    if (entry_count_ != 0) {
        fileInfo.set_avg_key_len(key_length_ / entry_count_);
        fileInfo.set_avg_value_len(value_length_ / entry_count_);
    }

    file_info_offset_ = index_offset_;
    index_offset_ = index_offset_ + fileInfo.EncodeToString().length();

    if (!fileInfo.WriteToFile(file_base_.get())) {
        LOG(ERROR)<< "fwrite error.";
        return false;
    }

    if (!index_->WriteToFile(file_base_.get())) {
        LOG(ERROR)<< "fwrite error, size: " << index_->EncodeToString().size();
        return false;
    }

    hfile::FileTrailer trailer;
    trailer.set_file_info_offset(file_info_offset_);
    trailer.set_data_index_offset(index_offset_);
    trailer.set_data_index_count(index_count_);
    trailer.set_total_uncompressed_bytes(total_bytes_);
    trailer.set_entry_count(entry_count_);
    trailer.set_compress_type(option_.compress_type());
    if (!trailer.WriteToFile(file_base_.get())) {
        LOG(ERROR)<< "fwrite error.";
        return false;
    }
    file_base_->Flush();
    file_base_.reset(NULL);
    return MoveToRealPath(option_.path());
}

bool UnsortedSSTableWriter::WriteBlockAndUpdateIndex() {
    ++index_count_;
    // Write the last block
    bool result = block_->WriteToFile(file_base_.get());
    index_->AddDataBlockInfo(block_->GetCompressedBufferSize(), block_->GetUncompressedBufferSize(),
                             first_key_);
    total_bytes_ += block_->GetUncompressedBufferSize();
    index_offset_ += block_->GetCompressedBufferSize();
    block_->ClearItems();
    if (!result) {
        LOG(ERROR)<< "WriteToFile error.";
    }
    return result;
}

}  // namespace toft
