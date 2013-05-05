// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_ENCODING_JSON_FORMAT_H_
#define TOFT_ENCODING_JSON_FORMAT_H_

#include <string>

#include "thirdparty/google/protobuf/message.h"

namespace google {
namespace protobuf {

// This class implements protocol buffer json format.  Printing and parsing
// protocol messages in json format is useful for javascript
class JsonFormat {
 public:
  static bool PrintToStyledString(const Message& message, string* output);
  static bool PrintToFastString(const Message& message, string* output);

  static bool ParseFromJsonString(const string& input, Message* output);

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(JsonFormat);
};

}  // namespace protobuf

}  // namespace google
#endif  // TOFT_ENCODING_JSON_FORMAT_H_
