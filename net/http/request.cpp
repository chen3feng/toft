// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 03/05/12
// Description: HttpRequest implementation

#include "toft/net/http/request.h"
#include "toft/base/string/algorithm.h"
#include "toft/base/string/concat.h"

#include "thirdparty/glog/logging.h"

namespace toft {

// NOTE: The order must be consistent with enum values because GetMethodName
// access this table by method_type enum as index
static const struct {
    HttpRequest::MethodType method;
    const char* method_name;
} kValidMethodNames[] = {
    { HttpRequest::METHOD_HEAD, "HEAD" },
    { HttpRequest::METHOD_GET, "GET" },
    { HttpRequest::METHOD_POST, "POST" },
    { HttpRequest::METHOD_PUT, "PUT" },
    { HttpRequest::METHOD_DELETE, "DELETE" },
    { HttpRequest::METHOD_OPTIONS, "OPTIONS" },
    { HttpRequest::METHOD_TRACE, "TRACE" },
    { HttpRequest::METHOD_CONNECT, "CONNECT" },
    { HttpRequest::METHOD_UNKNOWN, NULL },
};

void HttpRequest::Reset() {
    HttpMessage::Reset();
    m_method = METHOD_UNKNOWN;
    m_uri = "/";
}

// static
HttpRequest::MethodType HttpRequest::GetMethodByName(const char* method_name) {
    int i = 0;
    while (kValidMethodNames[i].method_name != NULL) {
        // Method is case sensitive.
        if (strcmp(method_name, kValidMethodNames[i].method_name) == 0) {
            return kValidMethodNames[i].method;
        }
        ++i;
    }
    return HttpRequest::METHOD_UNKNOWN;
}

// static
const char* HttpRequest::GetMethodName(MethodType method) {
    if (method <= METHOD_UNKNOWN || method >= METHOD_UPPER_BOUND) {
        return NULL;
    }
    return kValidMethodNames[method - 1].method_name; // Start from 1
}

bool HttpRequest::ParseStartLine(const StringPiece& data, HttpMessage::ErrorCode* error) {
    ErrorCode error_placeholder;
    if (error == NULL)
        error = &error_placeholder;

    std::vector<std::string> fields;
    SplitString(data, " ", &fields);
    if (fields.size() != 2 && fields.size() != 3) {
        *error = ERROR_START_LINE_NOT_COMPLETE;
        return false;
    }

    m_method = GetMethodByName(fields[0].c_str());
    if (m_method == METHOD_UNKNOWN) {
        *error = ERROR_METHOD_NOT_FOUND;
        return false;
    }
    m_uri = fields[1];

    if (fields.size() == 3) {
        if (!ParseVersion(fields[2])) {
            *error = ERROR_VERSION_UNSUPPORTED;
            return false;
        }
    }

    return true;
}

void HttpRequest::AppendStartLineToString(std::string* result) const {
    CHECK_NE(m_method, METHOD_UNKNOWN);
    StringAppend(result, GetMethodName(m_method), " ", m_uri);
    HttpVersion version = Version();
    if (!version.IsEmpty()) {
        StringAppend(result, " ", "HTTP/", version.Major(), ".", version.Minor());
    }
}

} // namespace toft

