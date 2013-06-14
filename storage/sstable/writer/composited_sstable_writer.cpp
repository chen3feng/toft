// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/writer/composited_sstable_writer.h"

#include "toft/base/string/algorithm.h"
#include "toft/base/string/number.h"
#include "toft/storage/sstable/merged_sstable_reader.h"
#include "toft/storage/sstable/writer/single_sstable_writer.h"
#include "toft/storage/sstable/writer/unsorted_sstable_writer.h"
#include "toft/system/time/clock.h"

#include "thirdparty/glog/logging.h"

DEFINE_string(tmp_dir_and_prefix, "/tmp/tmp_sstable",
              "dir and prefix for the CompositedSSTableWriter's "
              "middle temp sstables");
DEFINE_string(compress_type, "snappy", "compress_type used, should be snappy | none | lzo");

namespace toft {

CompositedSSTableWriter::CompositedSSTableWriter(const SSTableWriteOption &option)
                : SSTableWriter(option),
                  total_byte_(0),
                  curr_byte_(0),
                  batch_write_size_(512 * 1024 * 1024) {
    GetNewWriter();
}

CompositedSSTableWriter::CompositedSSTableWriter(const SSTableWriteOption &option,
                                                 const int64_t total_in_memory)
                : SSTableWriter(option),
                  total_byte_(0),
                  curr_byte_(0),
                  batch_write_size_(total_in_memory) {
    GetNewWriter();
}

CompositedSSTableWriter::~CompositedSSTableWriter() {
}

bool CompositedSSTableWriter::Add(const std::string &key, const std::string &value) {
    total_byte_ += key.length() + value.length();
    curr_byte_ += key.length() + value.length();
    if (!(builder_->Add(key, value))) {
        DeleteTmpFiles();
        return false;
    }
    if (curr_byte_ >= batch_write_size_) {
        if (!(builder_->Flush())) {
            DeleteTmpFiles();
            return false;
        }
        GetNewWriter();
        curr_byte_ = 0;
    }
    return true;
}

void CompositedSSTableWriter::AddMetaData(const std::string &key, const std::string &value) {
    file_info_meta_[key] = value;
}

bool CompositedSSTableWriter::Flush() {
    if (curr_byte_ >= 0) {
        if (!(builder_->Flush())) {
            DeleteTmpFiles();
            return false;
        }
    }
    MergedSSTableReader sstable;
    UnsortedSSTableWriter builder(option_);
    if (!sstable.Open(paths_, SSTableReader::ON_DISK, true)) {
        LOG(ERROR)<< "error open sstable!";
        DeleteTmpFiles();
        return false;
    }
    toft::scoped_ptr<SSTableReader::Iterator> iter(sstable.NewIterator());
    for (; iter->Valid(); iter->Next()) {
        if (!(builder.Add(iter->key(), iter->value()))) {
            DeleteTmpFiles();
            return false;
        }
    }
    std::map<std::string, std::string>::iterator it_fi_meta = file_info_meta_.begin();
    for (; it_fi_meta != file_info_meta_.end(); ++it_fi_meta) {
        builder.AddMetaData(it_fi_meta->first, it_fi_meta->second);
    }
    builder.Flush();
    DeleteTmpFiles();
    return true;
}

void CompositedSSTableWriter::DeleteTmpFiles() {
    std::vector<std::string>::iterator it_paths = paths_.begin();
    for (; it_paths != paths_.end(); ++it_paths) {
        if (remove((*it_paths).c_str()) != 0) {
            LOG(ERROR)<< "delete file failed: " << *it_paths;
        }
    }
}

void CompositedSSTableWriter::GetNewWriter() {
    CompressType compress_type = CompressType_kUnCompress;
    // TODO(yeshunping) : support more algorithms
    if (FLAGS_compress_type == "snappy") {
        compress_type = CompressType_kSnappy;
    } else if (FLAGS_compress_type == "lzo") {
        compress_type = CompressType_kLzo;
    } else if (FLAGS_compress_type == "none") {
        compress_type = CompressType_kUnCompress;
    }

    std::string path_suffix = option_.path();
    path_suffix = ReplaceAll(path_suffix, "/", "_");
    const std::string base = FLAGS_tmp_dir_and_prefix + path_suffix;
    std::string path = base + NumberToString(RealtimeClock.MicroSeconds());
    paths_.push_back(path);
    SSTableWriteOption option;
    option.set_compress_type(compress_type);
    option.set_path(path);
    builder_.reset(new SingleSSTableWriter(option));
}

}  // namespace toft
