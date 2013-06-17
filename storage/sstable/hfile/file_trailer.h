// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_HFILE_FILE_TRAILER_H
#define TOFT_STORAGE_SSTABLE_HFILE_FILE_TRAILER_H

#include <stdint.h>

#include <string>

#include "toft/storage/sstable/hfile/block.h"
#include "toft/storage/sstable/types.h"

namespace toft {
namespace hfile {

// A fixed trailer which contains offsets to other variable parts of the file.
// It contains basic metadata on this file too.
class FileTrailer : public Block {
    TOFT_DECLARE_UNCOPYABLE(FileTrailer);

public:
    FileTrailer();
    ~FileTrailer();

    virtual const std::string EncodeToString() const;
    virtual bool DecodeFromString(const std::string &str);

    // Please keep this updated.
    // TODO(yesp) : Remove this function ,and make magic as data member
    static int TrailerSize();

    // Getters and Setters
    int64_t file_info_offset() const {
        return file_info_offset_;
    }
    int64_t data_index_offset() const {
        return data_index_offset_;
    }
    int64_t meta_index_offset() const {
        return meta_index_offset_;
    }
    CompressType compress_type() const {
        return compress_type_;
    }
    int32_t entry_count() const {
        return entry_count_;
    }
    int32_t data_index_count() const {
        return data_index_count_;
    }

    void set_file_info_offset(int64_t offset) {
        file_info_offset_ = offset;
    }
    void set_data_index_offset(int64_t offset) {
        data_index_offset_ = offset;
    }
    void set_data_index_count(int32_t count) {
        data_index_count_ = count;
    }
    void set_total_uncompressed_bytes(int total_bytes) {
        total_uncompressed_bytes_ = total_bytes;
    }
    void set_entry_count(int entry_count) {
        entry_count_ = entry_count;
    }
    void set_compress_type(int codec) {
        compress_type_ = static_cast<CompressType>(codec);
    }

private:
    static const int kCurrentVersion = 1;
    // Offset to the fileinfo data, a small block of vitals..
    int64_t file_info_offset_;
    // Offset to the data block index.
    int64_t data_index_offset_;
    // How many index counts are there (aka: block count)
    int32_t data_index_count_;
    // Offset to the meta block index.
    int64_t meta_index_offset_;
    // How many meta block index entries (aka: meta block count)
    int32_t meta_index_count_;
    int64_t total_uncompressed_bytes_;
    int32_t entry_count_;
    CompressType compress_type_;
    int32_t version_;
};

}  // namespace hfile
}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_HFILE_FILE_TRAILER_H
