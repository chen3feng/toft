// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/base/string/format.h"
#include "toft/storage/sstable/sstable_reader.h"
#include "toft/storage/sstable/sstable_writer.h"

#include "thirdparty/glog/logging.h"

DEFINE_string(sstable_path,
              "toft/storage/sstable/test/testdata/test.sstable",
              "path of sstable");

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, false);
    toft::scoped_ptr<toft::SSTableReader> sstable(
                    toft::SSTableReader::Open(FLAGS_sstable_path, toft::SSTableReader::ON_DISK));
    CHECK(sstable.get());

    LOG(INFO)<< "testing NewIterator ...";
    toft::scoped_ptr<toft::SSTableReader::Iterator> iter(sstable->NewIterator());
    CHECK(iter.get());
    int i = 0;
    do {
        ++i;
        LOG(INFO)<< "key:" << iter->key();
        LOG(INFO)<< "value:" << iter->value();
        iter->Next();
    } while (iter->Valid());
}
