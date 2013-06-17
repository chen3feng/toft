// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/reader/sstable_reader_impl.h"

#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>

#include "toft/storage/file/file.h"
#include "toft/storage/sstable/reader/in_memory_sstable_reader.h"
#include "toft/storage/sstable/reader/on_disk_sstable_reader.h"
#include "toft/storage/sstable/sstable.h"

namespace toft {

// SSTableReader internal implementation.
bool SSTableReader::Impl::LoadFileInfo(File *file_base,
                                       hfile::DataIndex *data_index,
                                       hfile::FileInfo *file_info,
                                       hfile::FileTrailer *file_trailer) {
    // Load file trailer
    const int tailer_size = hfile::FileTrailer::TrailerSize();
    bool s = file_base->Seek(-tailer_size, SEEK_END);

    int64_t current_pos = file_base->Tell();
    file_base->Seek(0, SEEK_END);
    LOG(INFO)<< "current_pos:" << current_pos << ", file size:" << file_base->Tell();
    file_base->Seek(current_pos, SEEK_SET);

    if (!s) {
        LOG(ERROR)<< "Fail to seek";
        return false;
    }
    std::string buffer;
    buffer.resize(tailer_size);
    int64_t size = file_base->Read((void*) buffer.c_str(), tailer_size);
    if (tailer_size != size) {
        LOG(ERROR)<< "Read file trailer failed. read size:" << buffer.size()
        << ",TrailerSize: " << tailer_size;
        return false;
    }
    if (!file_trailer->DecodeFromString(buffer)) {
        LOG(ERROR)<< "parse file trailer failed, invalid format!";
        return false;
    }

    // if don't need file info, just return
    if (!file_info)
        return true;
    int64_t file_info_length = file_trailer->data_index_offset()
                    - file_trailer->file_info_offset();
    if (file_info_length < 0) {
        LOG(ERROR)<< "get invalid file info length: " << file_info_length;
        return false;
    }

    s = file_base->Seek(file_trailer->file_info_offset(), SEEK_SET);
    if (!s) {
        LOG(ERROR)<< "Fail to seek file";
        return false;
    }

    buffer.resize(file_info_length);
    size = file_base->Read((void*) buffer.c_str(), file_info_length);
    if (size != file_info_length) {
        LOG(ERROR)<< "read file info failed";
        return false;
    }

    if (!file_info->DecodeFromString(buffer)) {
        LOG(ERROR)<< "parse file info failed, invalid format!";
        return false;
    }

    // if don't need data index, just return
    if (!data_index)
        return true;
    int64_t index_length = 0;
    if (file_trailer->meta_index_offset() > 0) {
        index_length = file_trailer->meta_index_offset()
                        - file_trailer->data_index_offset();
    } else {
        // meta index missed
        if (!file_base->Seek(0, SEEK_END))
            index_length = -1;
        VLOG(4) << "data index offset: " << file_trailer->data_index_offset();
        index_length = file_base->Tell() - tailer_size - file_trailer->data_index_offset();
    }

    VLOG(2) << "index_length: " << index_length;
    if (index_length < 0) {
        LOG(ERROR)<< "get invalid data index length: " << index_length;
        return false;
    }
    s = file_base->Seek(file_trailer->data_index_offset(), SEEK_SET);
    if (!s) {
        LOG(ERROR)<< "Fail to seek file";
        return false;
    }
    buffer.resize(index_length);
    size = file_base->Read((void*) buffer.c_str(), index_length);
    if (index_length != size) {
        LOG(ERROR)<< "fread failed.";
        return false;
    }
    if (!data_index->DecodeFromString(buffer)) {
        LOG(ERROR)<< "parse data index failed, invalid format.";
        return false;
    }
    return true;
}

const std::string SSTableReader::Impl::FindValue(
                const std::string &key,
                const std::vector<std::pair<std::string, std::string> > &vec) {
    std::vector<std::pair<std::string, std::string> >::const_iterator iter = vec.begin();
    for (; iter != vec.end(); ++iter) {
        if (iter->first == key) {
            return iter->second;
        }
    }
    return "";
}

SSTableReader::Impl::Impl()
                : buffer_size_(0) {
    data_index_.reset(new hfile::DataIndex);
    file_trailer_.reset(new hfile::FileTrailer);
    file_info_.reset(new hfile::FileInfo);
}

SSTableReader::Impl::~Impl() {}

bool SSTableReader::Impl::LoadFile(const std::string &path) {
    CHECK(!file_base_.get()) << "the sstable is already opened.";
    path_ = path;
    MutexLocker l(&mutex_);
    file_base_.reset(File::Open(path, "r"));
    if (!file_base_.get()) {
        LOG(ERROR)<< "open sstable failed: " << path;
        return false;
    }

    return LoadFileInfo(file_base_.get(), data_index_.get(), file_info_.get(), file_trailer_.get());
}

const std::string SSTableReader::Impl::GetMetaData(const std::string &key) const {
    std::vector<std::pair<std::string, std::string> >::const_iterator iter = file_info_->meta_items
                    .begin();
    for (; iter != file_info_->meta_items.end(); ++iter) {
        if (iter->first == key) {
            return iter->second;
        }
    }
    return "";
}

void SSTableReader::Impl::IterMetaData(
                toft::Closure<bool(const std::string &, const std::string &)> *callback) {
    std::vector<std::pair<std::string, std::string> >::const_iterator iter = file_info_->meta_items
                    .begin();
    for (; iter != file_info_->meta_items.end(); iter++) {
        if (!callback->Run(iter->first, iter->second))
            return;
    }
}

bool SSTableReader::Impl::LoadDataBlock(int block_id, hfile::DataBlock *block) {
    CHECK(block_id >= 0 && block_id < data_index_->GetBlockSize())
                    << "invalid block_id: " << block_id;

    //  Get length for this block
    int64_t next_offset = 0;
    if (block_id + 1 < data_index_->GetBlockSize()) {
        next_offset = data_index_->GetOffset(block_id + 1);
    } else {
        next_offset = file_trailer_->file_info_offset();
    }
    int64_t cur_offset = data_index_->GetOffset(block_id);
    int length = next_offset - cur_offset;

    std::string buffer;
    {
        MutexLocker l(&mutex_);
        if (!file_base_->Seek(cur_offset, SEEK_SET)) {
            return false;
        }
        buffer.resize(length);
        int64_t size = file_base_->Read((void*) buffer.c_str(), length);
        if (size != length) {
            return false;
        }
    }
    return block->DecodeFromString(buffer);
}

}  // namespace toft
