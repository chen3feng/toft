// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/crypto/uuid/uuid.h"

#include <stdio.h>

#include "toft/base/string/algorithm.h"

#include "thirdparty/glog/logging.h"

namespace toft {

static const char uuidVersionRequired = '4';
static const int uuidVersionIdentifierIndex = 14;
static const char kUUIDFileName[] = "/proc/sys/kernel/random/uuid";

std::string CreateCanonicalUUIDString() {
    // This does not work for the linux system that turns on sandbox.
    FILE* fptr = fopen(kUUIDFileName, "r");
    if (!fptr) {
        LOG(ERROR) << "fail to open file: " << kUUIDFileName;
        return std::string();
    }
    char uuidStr[37];
    char* result = fgets(uuidStr, sizeof(uuidStr), fptr);
    fclose(fptr);
    if (!result) {
        LOG(ERROR) << "fail to read uuid string from :" << kUUIDFileName;
        return std::string();
    }
    std::string canonicalUuidStr(uuidStr);
    StringToLower(&canonicalUuidStr);
    CHECK(canonicalUuidStr[uuidVersionIdentifierIndex] == uuidVersionRequired);
    return canonicalUuidStr;
}

}  // namespace toft
