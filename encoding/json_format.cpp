// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/encoding/json_format.h"

#include <float.h>
#include <math.h>
#include <stdio.h>

#include <limits>
#include <stack>
#include <vector>

#include "thirdparty/glog/logging.h"
#include "thirdparty/google/protobuf/descriptor.h"
#include "thirdparty/jsoncpp/json.h"
#include "toft/base/string/number.h"

//  test for blade
namespace google {
namespace protobuf {

static Json::Value CreateNode(const FieldDescriptor* field,
                              const Reflection* reflection,
                              const Message& message) {
    Json::Value node;
    switch (field->cpp_type()) {
        case FieldDescriptor::CPPTYPE_INT32:
            node[field->name()] = reflection->GetInt32(message, field);
            break;
        case FieldDescriptor::CPPTYPE_INT64:
            node[field->name()] = toft::IntegerToString(reflection->GetInt64(message, field));
            break;
        case FieldDescriptor::CPPTYPE_UINT32:
            node[field->name()] = reflection->GetUInt32(message, field);
            break;
        case FieldDescriptor::CPPTYPE_UINT64:
            node[field->name()] = toft::IntegerToString(reflection->GetUInt64(message, field));
            break;
        case FieldDescriptor::CPPTYPE_DOUBLE:
            node[field->name()] = reflection->GetDouble(message, field);
            break;
        case FieldDescriptor::CPPTYPE_FLOAT:
            node[field->name()] = reflection->GetFloat(message, field);
            break;
        case FieldDescriptor::CPPTYPE_BOOL:
            node[field->name()] = reflection->GetBool(message, field);
            break;
        case FieldDescriptor::CPPTYPE_ENUM:
            node[field->name()] = reflection->GetEnum(message, field);
            break;
        case FieldDescriptor::CPPTYPE_STRING:
            node[field->name()] = reflection->GetString(message, field);
            break;
        case FieldDescriptor::CPPTYPE_MESSAGE:
            LOG(INFO)<< "message type, treat it later";
            break;
            default:
            CHECK(false) << "bad type:" << field->cpp_type();
            break;
        }
    return node;
}

static Json::Value CreateNodeOfRepeatedFiled(const FieldDescriptor* field,
                                             const Reflection* reflection,
                                             const Message& message,
                                             int index) {
    Json::Value node;
    switch (field->cpp_type()) {
        case FieldDescriptor::CPPTYPE_INT32:
            node[field->name()] = reflection->GetRepeatedInt32(message, field, index);
            break;
        case FieldDescriptor::CPPTYPE_INT64:
            node[field->name()] = toft::IntegerToString(
                            reflection->GetRepeatedInt64(message, field, index));
            break;
        case FieldDescriptor::CPPTYPE_UINT32:
            node[field->name()] = reflection->GetRepeatedUInt32(message, field, index);
            break;
        case FieldDescriptor::CPPTYPE_UINT64:
            node[field->name()] = toft::IntegerToString(
                            reflection->GetRepeatedUInt64(message, field, index));
            break;
        case FieldDescriptor::CPPTYPE_DOUBLE:
            node[field->name()] = reflection->GetRepeatedDouble(message, field, index);
            break;
        case FieldDescriptor::CPPTYPE_FLOAT:
            node[field->name()] = reflection->GetRepeatedFloat(message, field, index);
            break;
        case FieldDescriptor::CPPTYPE_BOOL:
            node[field->name()] = reflection->GetRepeatedBool(message, field, index);
            break;
        case FieldDescriptor::CPPTYPE_ENUM:
            node[field->name()] = reflection->GetRepeatedEnum(message, field, index);
            break;
        case FieldDescriptor::CPPTYPE_STRING:
            node[field->name()] = reflection->GetRepeatedString(message, field, index);
            break;
        case FieldDescriptor::CPPTYPE_MESSAGE:
            LOG(INFO)<< "message type, treat it later field name:" << field->name();
            break;
            default:
            CHECK(false) << "bad type:" << field->cpp_type();
            break;
        }
    return node;
}

static Json::Value MessageToJsonValue(const Message& message) {
    const Reflection* reflection = message.GetReflection();
    Json::Value root;
    Json::FastWriter writer;
    vector<const FieldDescriptor*> fields;
    reflection->ListFields(message, &fields);
    for (size_t i = 0; i < fields.size(); i++) {
        if (fields[i]->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE) {
            Json::Value node;
            if (fields[i]->is_repeated()) {
                int field_size = reflection->FieldSize(message, fields[i]);
                for (int k = 0; k < field_size; ++k) {
                    node.append(CreateNodeOfRepeatedFiled(fields[i], reflection, message, k));
                }
                root.append(node);
            } else {
                root.append(CreateNode(fields[i], reflection, message));
            }
        } else {
            const Message& sub_message = reflection->GetMessage(message, fields[i]);
            Json::Value sub_node;
            sub_node[fields[i]->name()] = MessageToJsonValue(sub_message);
            root.append(sub_node);
        }
    }
    return root;
}

bool JsonFormat::PrintToStyledString(const Message& message, string* output) {
    Json::Value root = MessageToJsonValue(message);
    output->assign(root.toStyledString());
    return true;
}

bool JsonFormat::PrintToFastString(const Message& message, string* output) {
    Json::Value root = MessageToJsonValue(message);
    Json::FastWriter writer;
    output->assign(writer.write(root));
    return true;
}

bool JsonFormat::ParseFromJsonString(const string& input, Message* output) {
    // TODO(yeshunping) : implement it
    Json::Reader reader;
    Json::Value root;
    reader.parse(input, root, false);
    LOG(INFO)<< root.toStyledString();

    return true;
}

}  // namespace protobuf
}  // namespace google
