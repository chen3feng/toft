// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/encoding/proto_util.h"

#include <string>

#include "toft/base/static_class.h"
#include "toft/storage/file/file.h"

#include "thirdparty/protobuf/text_format.h"

namespace toft {

bool LoadMessageFromTextFile(const std::string& filename,
                             google::protobuf::Message* out) {
    std::string content;
    if (!File::ReadAll(filename, &content)) {
        LOG(ERROR)<< "fail to read file:" << filename;
        return false;
    }
    return google::protobuf::TextFormat::ParseFromString(content, out);
}

}  // namespace toft
