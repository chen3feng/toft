// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_ENCODING_PROTO_UTIL_H_
#define TOFT_ENCODING_PROTO_UTIL_H_

#include <string>

#include "toft/base/static_class.h"

#include "thirdparty/protobuf/text_format.h"

namespace toft {

struct ProtoUtil {
    TOFT_DECLARE_STATIC_CLASS(ProtoUtil);

public:
static bool ParseFromASCIIString(const std::string& debug_str,
                                 google::protobuf::Message* out) {
  google::protobuf::TextFormat::Parser parser;
  return parser.ParseFromString(debug_str, out);
}
};

}  // namespace toft
#endif  // TOFT_ENCODING_PROTO_UTIL_H_
