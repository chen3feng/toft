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
HttpRequest::MethodType HttpRequest::GetMethodByName(StringPiece method_name) {
    int i = 0;
    while (kValidMethodNames[i].method_name != NULL) {
        // Method is case sensitive.
        if (method_name == kValidMethodNames[i].method_name) {
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

    static const size_t kMinHttpMethodLength = 3;
    size_t pos = data.find(' ', kMinHttpMethodLength);
    if (pos == StringPiece::npos) {
        *error = ERROR_START_LINE_NOT_COMPLETE;
        return false;
    }

    StringPiece method = data.substr(0, pos);
    StringTrim(&method);
    m_method = GetMethodByName(method);
    if (m_method == METHOD_UNKNOWN) {
        *error = ERROR_METHOD_NOT_FOUND;
        return false;
    }

    size_t prev_pos = pos + 1;
    pos = data.find(' ', prev_pos);
    StringPiece uri;
    if (pos == StringPiece::npos) {
        uri = data.substr(prev_pos);
    } else {
        uri = data.substr(prev_pos, pos - prev_pos);
    }
    StringTrim(&uri);
    uri.copy_to_string(&m_uri);

    if (pos != StringPiece::npos) {
        StringPiece version = data.substr(pos);
        StringTrim(&version);
        if (!ParseVersion(version)) {
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

