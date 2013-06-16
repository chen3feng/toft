// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_HFILE_BLOCK_H
#define TOFT_STORAGE_SSTABLE_HFILE_BLOCK_H

#include <string>

#include "toft/base/uncopyable.h"

namespace toft {
class File;

namespace hfile {
class Block {
    TOFT_DECLARE_UNCOPYABLE(Block);

public:
    Block() {
    }
    virtual ~Block() = 0;

    // Append the block into the file.
    bool WriteToFile(File *fb);

    virtual const std::string EncodeToString() const {
        return std::string();
    }
    virtual bool DecodeFromString(const std::string &str) = 0;
};

}  // namespace hfile
}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_HFILE_BLOCK_H
