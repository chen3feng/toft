// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_ENCODING_PROTO_UTIL_H_
#define TOFT_ENCODING_PROTO_UTIL_H_

#include <string>

#include "toft/base/static_class.h"

namespace google {
namespace protobuf {
class Message;
}
}

namespace toft {

bool LoadMessageFromTextFile(const std::string& filename, google::protobuf::Message* out);

}  // namespace toft
#endif  // TOFT_ENCODING_PROTO_UTIL_H_
