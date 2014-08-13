// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/encoding/proto_json_format.h"

#include <math.h>
#include <stdlib.h>
#include <limits>

#include "thirdparty/glog/logging.h"
#include "thirdparty/google/protobuf/text_format.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/jsoncpp/json.h"
#include "toft/encoding/proto_json_format_test.pb.h"
#include "toft/storage/file/file.h"

namespace toft {

TEST(JsonFormtTest, PrintToJson) {
    toft::Person p;
    p.set_age(30);
    toft::NameInfo* name = p.mutable_name();
    name->set_first_name("Ye");
    name->set_second_name("Shunping");
    p.set_address("beijing");
    p.add_phone_number("15100000000");
    p.add_phone_number("15100000001");
    p.set_address_id(434798436777434024L);
    p.set_people_type(toft::HAN_ZU);
    LOG(INFO)<< "Text format for Message:\n" << p.Utf8DebugString();

    std::string styled_str;
    ProtoJsonFormat::PrintToStyledString(p, &styled_str);
    std::string expected_styled_str;
    toft::File::ReadAll("json_styled_string.txt", &expected_styled_str);
    EXPECT_EQ(styled_str, expected_styled_str);

    std::string fast_str;
    ProtoJsonFormat::PrintToFastString(p, &fast_str);
    VLOG(3)<< "FastString json format for Message:\n" << fast_str;
    std::string expected_fast_str;
    toft::File::ReadAll("json_fast_string.txt", &expected_fast_str);
    EXPECT_EQ(fast_str, expected_fast_str);
}

void TestJsonString(const std::string& json_file) {
    std::string styled_str;
    toft::File::ReadAll(json_file, &styled_str);
    toft::Person pb;
    ProtoJsonFormat::ParseFromString(styled_str, &pb);

    toft::Person expected_pb;
    {
        std::string content;
        std::string path = "debug_string.txt";
        toft::File::ReadAll(path, &content);
        google::protobuf::TextFormat::ParseFromString(content, &expected_pb);
    }
    EXPECT_EQ(pb.SerializeAsString(), expected_pb.SerializeAsString());
}

TEST(JsonFormtTest, ParseFromFastJsonString) {
    std::string path = "json_fast_string.txt";
    TestJsonString(path);
}

TEST(JsonFormtTest, ParseFromStyledJsonString) {
    std::string path = "json_styled_string.txt";
    TestJsonString(path);
}
}  // namespace toft
