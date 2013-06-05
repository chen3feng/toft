// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_COMPRESS_SNAPPY_H
#define TOFT_COMPRESS_SNAPPY_H

#include "toft/compress/compress.h"

namespace toft {

class SnappyCompression : public Compression {
 public:
  SnappyCompression();
  virtual ~SnappyCompression();

  virtual bool Compress(const char* str,
                        size_t length,
                        std::string* out);
  virtual bool Uncompress(const char* str,
                          size_t length,
                          std::string* out);
  virtual void GetAlgorithmName(std::string* out) {
    out->assign("snappy");
  }

private:
    TOFT_DECLARE_UNCOPYABLE(SnappyCompression);
};
}  // namespace toft
#endif  // TOFT_COMPRESS_SNAPPY_H
