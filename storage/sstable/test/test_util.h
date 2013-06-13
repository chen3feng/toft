// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_STORAGE_SSTABLE_TEST_TEST_UTIL_H_
#define TOFT_STORAGE_SSTABLE_TEST_TEST_UTIL_H_

#include <string>

#include "thirdparty/gtest/gtest.h"

namespace toft {

static const int kTestNum = 1000;
static const int kMaxLength = 50;

const std::string GenKey(int i, int mod) {
    char subfix = 'a' + i % 26;
    int length = i % mod;
    std::string key;
    //  key.reserve(length + 9);
    key = StringPrint("%09d", i);
    for (int j = 0; j < length; ++j) {
        key += subfix;
    }
    return key;
}

const std::string GenValue(int i, int mod) {
    return GenKey(i, mod) + "_value";
}

}  // namespace toft

#endif  // TOFT_STORAGE_SSTABLE_TEST_TEST_UTIL_H_
