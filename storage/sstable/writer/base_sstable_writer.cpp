// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/writer/base_sstable_writer.h"

#include "toft/hash/fingerprint.h"
#include "toft/storage/file/file.h"
#include "toft/storage/path/path.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_string(temp_sstable_dir, "/tmp",
              "dir for the temp sstable file that is used for building phrase");

namespace toft {

SSTableWriter::SSTableWriter(const SSTableWriteOption &option)
                : option_(option) {
}

bool SSTableWriter::Add(const std::string &key, const std::string &value) {
    return false;
}

void SSTableWriter::AddOrDie(const std::string &key, const std::string &value) {
    CHECK(Add(key, value)) << "add entry error!";
}

std::string SSTableWriter::GetTempSSTablePath(const std::string &path) {
    std::string base_path =
        Path::Join(FLAGS_temp_sstable_dir, Fingerprint64ToString(Fingerprint64(path)));
    return base_path + ".sstmp";
}

bool SSTableWriter::MoveToRealPath(const std::string &path) {
    return File::Rename(GetTempSSTablePath(path), path);
}

}  // namespace toft
