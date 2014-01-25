// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/encoding/proto_json_format.h"

#include <stdio.h>

#include <limits.h>
#include <string>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "thirdparty/google/protobuf/descriptor.h"
#include "thirdparty/google/protobuf/message.h"
#include "thirdparty/jsoncpp/json.h"
#include "toft/base/string/number.h"

namespace toft {

using google::protobuf::Descriptor;
using google::protobuf::EnumValueDescriptor;
using google::protobuf::EnumDescriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::Message;
using google::protobuf::Reflection;

static bool SetValueForMessage(const std::string& field_name,
                               const Json::Value& value,
                               Message* pb);
static bool ParseFromJsonValue(const Json::Value& root, Message* pb);

static void CreateNode(const FieldDescriptor* field,
                       const Reflection* reflection,
                       const Message& message,
                       Json::Value* node) {
    switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
        (*node)[field->name()] = reflection->GetInt32(message, field);
        break;
    case FieldDescriptor::CPPTYPE_INT64:
        (*node)[field->name()] = toft::IntegerToString(reflection->GetInt64(message, field));
        break;
    case FieldDescriptor::CPPTYPE_UINT32:
        (*node)[field->name()] = reflection->GetUInt32(message, field);
        break;
    case FieldDescriptor::CPPTYPE_UINT64:
        (*node)[field->name()] = toft::IntegerToString(reflection->GetUInt64(message, field));
        break;
    case FieldDescriptor::CPPTYPE_DOUBLE:
        (*node)[field->name()] = reflection->GetDouble(message, field);
        break;
    case FieldDescriptor::CPPTYPE_FLOAT:
        (*node)[field->name()] = reflection->GetFloat(message, field);
        break;
    case FieldDescriptor::CPPTYPE_BOOL:
        (*node)[field->name()] = reflection->GetBool(message, field);
        break;
    case FieldDescriptor::CPPTYPE_ENUM:
        (*node)[field->name()] = reflection->GetEnum(message, field)->number();
        break;
    case FieldDescriptor::CPPTYPE_STRING:
        (*node)[field->name()] = reflection->GetString(message, field);
        break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
        CHECK(false)<< "not reachable";
        break;
    default:
        CHECK(false) << "bad type:" << field->cpp_type();
        break;
    }
}

static void CreateNodeOfRepeatedFiled(const FieldDescriptor* field,
                                      const Reflection* reflection,
                                      const Message& message,
                                      int index,
                                      Json::Value* node) {
    switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
        node->append(reflection->GetRepeatedInt32(message, field, index));
        break;
    case FieldDescriptor::CPPTYPE_INT64:
        node->append(toft::IntegerToString(
                        reflection->GetRepeatedInt64(message, field, index)));
        break;
    case FieldDescriptor::CPPTYPE_UINT32:
        node->append(reflection->GetRepeatedUInt32(message, field, index));
        break;
    case FieldDescriptor::CPPTYPE_UINT64:
        node->append(toft::IntegerToString(
                        reflection->GetRepeatedUInt64(message, field, index)));
        break;
    case FieldDescriptor::CPPTYPE_DOUBLE:
        node->append(reflection->GetRepeatedDouble(message, field, index));
        break;
    case FieldDescriptor::CPPTYPE_FLOAT:
        node->append(reflection->GetRepeatedFloat(message, field, index));
        break;
    case FieldDescriptor::CPPTYPE_BOOL:
        node->append(reflection->GetRepeatedBool(message, field, index));
        break;
    case FieldDescriptor::CPPTYPE_ENUM:
        node->append(reflection->GetRepeatedEnum(message, field, index));
        break;
    case FieldDescriptor::CPPTYPE_STRING:
        node->append(reflection->GetRepeatedString(message, field, index));
        break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
        CHECK(false)<< "not reachable";
        break;
    default:
        CHECK(false) << "bad type:" << field->cpp_type();
        break;
    }
}

bool ProtoJsonFormat::WriteToValue(const Message& message, Json::Value* root) {
    const Reflection* reflection = message.GetReflection();
    Json::FastWriter writer;
    std::vector<const FieldDescriptor*> fields;
    reflection->ListFields(message, &fields);

    for (size_t i = 0; i < fields.size(); i++) {
        const FieldDescriptor* field = fields[i];
        const std::string& field_name = field->name();

        if (field->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE) {
            if (field->is_repeated()) {
                Json::Value node;
                int field_size = reflection->FieldSize(message, field);
                for (int k = 0; k < field_size; ++k) {
                    CreateNodeOfRepeatedFiled(field, reflection, message, k, &node);
                }
                (*root)[field_name] = node;
            } else {
                CreateNode(field, reflection, message, root);
            }
      } else {
          if (field->is_repeated()) {
              Json::Value node;
              for (int i = 0; i < reflection->FieldSize(message, field); ++i) {
                  Json::Value sub_node;
                  const Message& sub_message = reflection->GetRepeatedMessage(message,
                                                                              field, i);
                  WriteToValue(sub_message, &sub_node);
                  node.append(sub_node);
              }
              (*root)[field_name] = node;
          } else {
              const Message& sub_message = reflection->GetMessage(message, field);
              WriteToValue(sub_message, &((*root)[field_name]));
          }
      }
    }
    return true;
}

bool ProtoJsonFormat::PrintToStyledString(const Message& message, std::string* output) {
    Json::Value root;
    WriteToValue(message, &root);
    output->assign(root.toStyledString());
    return true;
}

bool ProtoJsonFormat::PrintToFastString(const Message& message, std::string* output) {
    Json::Value root;
    WriteToValue(message, &root);
    Json::FastWriter writer;
    output->assign(writer.write(root));
    return true;
}

static bool SetSingleValueForMessage(const Reflection* reflection,
                                     Message* pb,
                                     const FieldDescriptor* field,
                                     const Json::Value& node) {
    switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
        reflection->SetInt32(pb, field, node.asInt());
        break;
    case FieldDescriptor::CPPTYPE_INT64: {
        int64_t number = 0;
        if (!toft::StringToNumber(node.asString(), &number)) {
            LOG(WARNING) << "Fail to convert to interger:" << node.asString();
            return false;
        }
        reflection->SetInt64(pb, field, number);
        break;
    }
    case FieldDescriptor::CPPTYPE_UINT32:
        reflection->SetUInt32(pb, field, node.asUInt());
        break;
    case FieldDescriptor::CPPTYPE_UINT64: {
        uint64_t number = 0;
        if (!toft::StringToNumber(node.asString(), &number)) {
            LOG(WARNING) << "Fail to convert to interger:" << node.asString();
            return false;
        }
        reflection->SetUInt64(pb, field, number);
        break;
    }
    case FieldDescriptor::CPPTYPE_DOUBLE:
        reflection->SetDouble(pb, field, node.asDouble());
        break;
    case FieldDescriptor::CPPTYPE_FLOAT:
        reflection->SetFloat(pb, field, node.asDouble());
        break;
    case FieldDescriptor::CPPTYPE_BOOL:
        reflection->SetBool(pb, field, node.asBool());
        break;
    case FieldDescriptor::CPPTYPE_ENUM: {
        const EnumValueDescriptor* enum_value = reflection->GetEnum(*pb, field);
        const EnumDescriptor* enum_desc = enum_value->type();
        const EnumValueDescriptor* real_enum_value = enum_desc->FindValueByNumber(node.asInt());
        reflection->SetEnum(pb, field, real_enum_value);
        break;
    }
    case FieldDescriptor::CPPTYPE_STRING:
        reflection->SetString(pb, field, node.asString());
        break;
    case FieldDescriptor::CPPTYPE_MESSAGE: {
        Message* sub_pb = reflection->MutableMessage(pb, field);
        if (!ParseFromJsonValue(node, sub_pb)) {
            return false;
        }
        break;
    }
    default:
        CHECK(false) << "bad type:" << field->cpp_type();
        break;
    }
    return true;
}

static bool SetRepeatedValueForMessage(const Reflection* reflection,
                                       Message* pb,
                                       const FieldDescriptor* field,
                                       const Json::Value& sub_node) {
    switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
        reflection->AddInt32(pb, field, sub_node.asInt());
        break;
    case FieldDescriptor::CPPTYPE_INT64: {
        int64_t number = 0;
        if (!toft::StringToNumber(sub_node.asString(), &number)) {
            LOG(WARNING) << "Fail to convert to interger:" << sub_node.asString();
            return false;
        }
        reflection->AddInt64(pb, field, number);
        break;
    }
    case FieldDescriptor::CPPTYPE_UINT32:
        reflection->AddUInt32(pb, field, sub_node.asUInt());
        break;
    case FieldDescriptor::CPPTYPE_UINT64: {
        uint64_t number = 0;
        if (!toft::StringToNumber(sub_node.asString(), &number)) {
            LOG(WARNING) << "Fail to convert to interger:" << sub_node.asString();
            return false;
        }
        reflection->AddUInt64(pb, field, number);
        break;
    }
    case FieldDescriptor::CPPTYPE_DOUBLE:
        reflection->AddDouble(pb, field, sub_node.asDouble());
        break;
    case FieldDescriptor::CPPTYPE_FLOAT:
        reflection->AddFloat(pb, field, sub_node.asDouble());
        break;
    case FieldDescriptor::CPPTYPE_BOOL:
        reflection->AddBool(pb, field, sub_node.asBool());
        break;
    case FieldDescriptor::CPPTYPE_ENUM: {
        const EnumValueDescriptor* enum_value = reflection->GetEnum(*pb, field);
        const EnumDescriptor* enum_desc = enum_value->type();
        const EnumValueDescriptor* real_enum_value = enum_desc->FindValueByNumber(sub_node.asInt());
        reflection->AddEnum(pb, field, real_enum_value);
        break;
    }
    case FieldDescriptor::CPPTYPE_STRING:
        reflection->AddString(pb, field, sub_node.asString());
        break;
    case FieldDescriptor::CPPTYPE_MESSAGE:
        LOG(INFO)<< "Not implemented";
        break;
    default:
        LOG(FATAL) << "Bad type:" << field->cpp_type();
        break;
    }
    return true;
}

static bool SetValueForMessage(const std::string& field_name,
                               const Json::Value& value,
                               Message* pb) {
    const Reflection* reflection =  pb->GetReflection();
    const Descriptor* descriptor = pb->GetDescriptor();
    const FieldDescriptor* field = descriptor->FindFieldByName(field_name);
    if (field == NULL) {
        LOG(ERROR) << "No field:" << field_name << ", type:" << pb->GetTypeName();
        return false;
    }
    if (field->is_repeated()) {
        if (value.type() != Json::arrayValue) {
            LOG(INFO) << "Expect array, but real time is:" << value.type();
            return false;
        }
        Json::Value::const_iterator it = value.begin();
        for (; it != value.end(); ++it) {
            SetRepeatedValueForMessage(reflection, pb, field, *it);
        }
        return true;
    } else {
        return SetSingleValueForMessage(reflection, pb, field, value);
    }
}

static bool ParseFromJsonValue(const Json::Value& root, Message* pb) {
    std::string pb_type = pb->GetTypeName();
    VLOG(20) << "Set info for type:" << pb_type;

    Json::Value::Members members = root.getMemberNames();
    for (size_t i = 0; i < members.size(); ++i) {
        VLOG(21) << "Member:" << members[i];
        const std::string& field_name = members[i];
        Json::Value sub_node;
        sub_node = root.get(field_name, sub_node);
        if (sub_node.isNull()) {
            const FieldDescriptor* field = pb->GetDescriptor()->FindFieldByName(field_name);
            if (field && field->is_required()) {
                LOG(ERROR) << "Missing required field:" << field_name;
                return false;
            }
        } else {
            if (!SetValueForMessage(field_name, sub_node, pb)) {
                return false;
            }
        }
    }
    return true;
}

bool ProtoJsonFormat::ParseFromString(const std::string& input, Message* pb) {
    Json::Reader reader;
    Json::Value root;
    reader.parse(input, root, false);
    return ParseFromJsonValue(root, pb);
}

bool ProtoJsonFormat::ParseFromValue(const Json::Value& input, Message* output) {
    return ParseFromJsonValue(input, output);
}
}  // namespace toft
