// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_WRITER_SINGLE_SSTABLE_WRITER_H
#define TOFT_STORAGE_SSTABLE_WRITER_SINGLE_SSTABLE_WRITER_H

#include <deque>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "toft/base/scoped_ptr.h"
#include "toft/storage/sstable/types.h"
#include "toft/storage/sstable/writer/base_sstable_writer.h"

namespace toft {
class File;

class SingleSSTableWriter : public SSTableWriter {
    TOFT_DECLARE_UNCOPYABLE(SingleSSTableWriter);

public:
    explicit SingleSSTableWriter(const SSTableWriteOption &option);
    ~SingleSSTableWriter();

    //  add data block
    virtual bool Add(const std::string &key, const std::string &value);
    //  add meta data
    virtual void AddMetaData(const std::string &key, const std::string &value);
    //  flush to disk
    virtual bool Flush();

private:
    std::vector<std::deque<std::pair<std::string, std::string> >::iterator> data_index_;  // NOLINT
    std::deque<std::pair<std::string, std::string> > d_data_;

    std::map<std::string, std::string> file_info_meta_;

    toft::scoped_ptr<File> file_base_;
    toft::scoped_ptr<hfile::DataBlock> block_;
    toft::scoped_ptr<hfile::DataIndex> index_;
    std::string first_key_;
    int entry_count_;
    int64_t total_bytes_;
    int64_t index_offset_;
    int index_count_;
    // Used calculating average key and value lengths.
    int64_t key_length_;
    int64_t value_length_;
    int64_t file_info_offset_;
    std::string last_key_;
    bool flushed_;
};

}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_WRITER_SINGLE_SSTABLE_WRITER_H
