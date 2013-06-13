// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/sstable_reader.h"

namespace toft {

SSTableReader::Iterator::Iterator()
                : valid_(false) {
}

SSTableReader::Iterator::~Iterator() {
}

const std::string SSTableReader::Iterator::key() const {
    return key_;
}

const std::string SSTableReader::Iterator::value() const {
    return value_;
}

bool SSTableReader::Iterator::Valid() const {
    return valid_;
}

}  // namespace toft
