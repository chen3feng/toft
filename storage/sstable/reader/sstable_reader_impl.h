// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_READER_SSTABLE_READER_IMPL_H
#define TOFT_STORAGE_SSTABLE_READER_SSTABLE_READER_IMPL_H

#include "toft/storage/sstable/sstable_reader.h"

#include <stdint.h>

#include <string>
#include <utility>
#include <vector>

#include "toft/base/closure.h"
#include "toft/base/scoped_ptr.h"
#include "toft/storage/sstable/sstable.h"
#include "toft/system/threading/mutex.h"

namespace toft {

class SSTableReader::Impl {
    TOFT_DECLARE_UNCOPYABLE(Impl);

public:
    // Only load the file info, but not data.
    static bool LoadFileInfo(File *file_base, hfile::DataIndex *data_index,
                             hfile::FileInfo *file_info,
                             hfile::FileTrailer *file_trailer);

    static const std::string FindValue(
                    const std::string &key,
                    const std::vector<std::pair<std::string, std::string> > &vec);

    Impl();
    ~Impl();

    bool LoadDataBlock(int block_id, hfile::DataBlock *block);

    bool LoadFile(const std::string &path);

    const std::string GetMetaData(const std::string &key) const;

    void IterMetaData(toft::Closure<bool(const std::string&, const std::string&)>* callback);

    int EntryCount() const {
        return file_trailer_->entry_count();
    }

    toft::scoped_ptr<hfile::FileTrailer> file_trailer_;
    toft::scoped_ptr<hfile::DataIndex> data_index_;
    std::string path_;

private:
    toft::scoped_ptr<hfile::FileInfo> file_info_;
    uint32_t buffer_size_;

    toft::Mutex mutex_;  // protects file_base_
    toft::scoped_ptr<File> file_base_;
};

}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_READER_SSTABLE_READER_IMPL_H
