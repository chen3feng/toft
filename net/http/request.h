// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 03/05/12
// Description: HttpRequest class declaration

#ifndef TOFT_NET_HTTP_REQUEST_H
#define TOFT_NET_HTTP_REQUEST_H
#pragma once

#include <algorithm>
#include <string>
#include "toft/net/http/message.h"

namespace toft {

// Describes a http request.
class HttpRequest : public HttpMessage {
public:
    enum MethodType {
        METHOD_UNKNOWN = 0,
        METHOD_HEAD,
        METHOD_GET,
        METHOD_POST,
        METHOD_PUT,
        METHOD_DELETE,
        METHOD_OPTIONS,
        METHOD_TRACE,
        METHOD_CONNECT,
        METHOD_UPPER_BOUND,  // no use, just label the bound.
    };

    HttpRequest() : m_method(METHOD_UNKNOWN), m_uri("/") {
    }
    ~HttpRequest() {}
    virtual void Reset();

public:
    static MethodType GetMethodByName(StringPiece method_name);
    static const char* GetMethodName(MethodType method);

    MethodType Method() const { return m_method; }
    void SetMethod(MethodType method) {
        m_method = method;
    }

    const std::string& Uri() const { return m_uri; }
    void SetUri(const std::string& uri) {
        m_uri = uri;
    }

    void Swap(HttpRequest* other) {
        HttpMessage::Swap(other);
        using std::swap;
        swap(m_method, other->m_method);
        swap(m_uri, other->m_uri);
    }

private:
    virtual void AppendStartLineToString(std::string* result) const;
    virtual bool ParseStartLine(const StringPiece& data, ErrorCode* error = NULL);

    MethodType m_method;
    std::string m_uri;
};

} // namespace toft

// adapt to std::swap
namespace std {

template <>
inline void swap(toft::HttpRequest& lhs, toft::HttpRequest& rhs) {
    lhs.Swap(&rhs);
}

} // namespace std

#endif // TOFT_NET_HTTP_REQUEST_H
