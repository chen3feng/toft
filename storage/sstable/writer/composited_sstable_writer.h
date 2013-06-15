// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_WRITER_COMPOSITED_SSTABLE_WRITER_H
#define TOFT_STORAGE_SSTABLE_WRITER_COMPOSITED_SSTABLE_WRITER_H

#include <map>
#include <string>
#include <vector>

#include "toft/base/scoped_ptr.h"
#include "toft/storage/sstable/writer/base_sstable_writer.h"

namespace toft {
class SingleSSTableWriter;

class CompositedSSTableWriter : public SSTableWriter {
    TOFT_DECLARE_UNCOPYABLE(CompositedSSTableWriter);

public:
    explicit CompositedSSTableWriter(const SSTableWriteOption &option);
    CompositedSSTableWriter(const SSTableWriteOption &option,
                            const int64_t total_in_memory);

    ~CompositedSSTableWriter();

    virtual bool Add(const std::string &key, const std::string &value);
    virtual void AddMetaData(const std::string &key, const std::string &value);
    virtual bool Flush();

private:
    void GetNewWriter();
    void DeleteTmpFiles();

    int64_t total_byte_;
    int64_t curr_byte_;
    std::vector<std::string> paths_;
    std::map<std::string, std::string> file_info_meta_;
    toft::scoped_ptr<SingleSSTableWriter> builder_;
    const int64_t batch_write_size_;
};

}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_WRITER_COMPOSITED_SSTABLE_WRITER_H
