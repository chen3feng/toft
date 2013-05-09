// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/encoding/json_format.h"

#include <math.h>
#include <stdlib.h>
#include <limits>

#include "thirdparty/glog/logging.h"
#include "thirdparty/google/protobuf/text_format.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/jsoncpp/json.h"
#include "toft/encoding/unittest.pb.h"
#include "toft/storage/file/file.h"

namespace google {
namespace protobuf {

TEST(JsonFormatUnittest, PrintToJson) {
    Person p;
    p.set_age(30);
    NameInfo* name = p.mutable_name();
    name->set_first_name("Ye");
    name->set_second_name("Shunping");
    p.set_address("beijing");
    p.add_phone_number("15100000000");
    p.add_phone_number("15100000001");
    p.set_address_id(434798436777434024L);
    p.set_people_type(HAN_ZU);
    LOG(INFO)<< "text format for Message:\n" << p.Utf8DebugString();

    string styled_str;
    google::protobuf::JsonFormat::PrintToStyledString(p, &styled_str);
    string expected_styled_str;
    toft::File::ReadAll("json_styled_string.txt", &expected_styled_str);
    EXPECT_EQ(styled_str, expected_styled_str);

    string fast_str;
    google::protobuf::JsonFormat::PrintToFastString(p, &fast_str);
    VLOG(3)<< "FastString json format for Message:\n" << fast_str;
    string expected_fast_str;
    toft::File::ReadAll("json_fast_string.txt", &expected_fast_str);
    EXPECT_EQ(fast_str, expected_fast_str);
}

void TestJsonString(const string& json_file) {
    string styled_str;
    toft::File::ReadAll(json_file, &styled_str);
    Person pb;
    google::protobuf::JsonFormat::ParseFromJsonString(styled_str, &pb);

    Person expected_pb;
    {
        string content;
        string path = "debug_string.txt";
        toft::File::ReadAll(path, &content);
        google::protobuf::TextFormat::ParseFromString(content, &expected_pb);
    }
    EXPECT_EQ(pb.SerializeAsString(), expected_pb.SerializeAsString());
}

TEST(JsonFormatUnittest, ParseFromFastJsonString) {
    string path = "json_fast_string.txt";
    TestJsonString(path);
}

TEST(JsonFormatUnittest, ParseFromStyledJsonString) {
    string path = "json_styled_string.txt";
    TestJsonString(path);
}
}  // namespace protobuf
}  // namespace google
