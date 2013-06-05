// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_COMPRESS_COMPRESS_H_
#define TOFT_COMPRESS_COMPRESS_H_

#include <string>

#include "toft/base/uncopyable.h"
#include "toft/base/class_registry.h"

namespace toft {

class Compression {
public:
    Compression() {}
    virtual ~Compression() {}

    virtual bool Compress(const char* str, size_t length, std::string* out) = 0;

    virtual bool Uncompress(const char* str, size_t length, std::string* out) = 0;

    virtual void GetAlgorithmName(std::string* out) = 0;

private:
    TOFT_DECLARE_UNCOPYABLE(Compression);
};

TOFT_CLASS_REGISTRY_DEFINE(compression_registry, Compression);

#define REGISTER_COMPRESSION(class_name) \
    TOFT_CLASS_REGISTRY_REGISTER_CLASS( \
        toft::compression_registry, \
        toft::Compression, \
        #class_name, \
        class_name)

#define CREATE_COMPRESSION(name_as_string) \
    TOFT_CLASS_REGISTRY_CREATE_OBJECT(compression_registry, name_as_string)

}  // namespace toft
#endif  // TOFT_COMPRESS_COMPRESS_H_
