// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_ENCODING_PROTO_JSON_FORMAT_H_
#define TOFT_ENCODING_PROTO_JSON_FORMAT_H_

#include <string>

#include "toft/base/uncopyable.h"

namespace Json {
class Value;
}
namespace google {
namespace protobuf {
class Message;
}
}

namespace toft {

// This class implements protocol buffer json format.  Printing and parsing
// protocol messages in json format is useful for javascript
class ProtoJsonFormat {
public:
    static bool PrintToStyledString(const google::protobuf::Message& message,
                                    std::string* output);

    static bool PrintToFastString(const google::protobuf::Message& message,
                                  std::string* output);

    static bool WriteToValue(const google::protobuf::Message& message,
                             Json::Value* output);

    static bool ParseFromValue(const Json::Value& input,
                               google::protobuf::Message* output);

    static bool ParseFromString(const std::string& input,
                                google::protobuf::Message* output);

private:
    TOFT_DECLARE_UNCOPYABLE(ProtoJsonFormat);
};

}  // namespace toft

#endif  // TOFT_ENCODING_PROTO_JSON_FORMAT_H_
