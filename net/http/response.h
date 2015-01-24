// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 03/05/12
// Description: HttpResponse class declaration

#ifndef TOFT_NET_HTTP_RESPONSE_H
#define TOFT_NET_HTTP_RESPONSE_H
#pragma once

#include <algorithm>
#include <string>
#include "toft/net/http/message.h"

namespace toft {

// Describes a http response.
class HttpResponse : public HttpMessage {
public:
    // See RFC2616: 10 Status Code Definition
    enum StatusCode {
        Status_None               = 0,
        Status_Continue           = 100,
        Status_SwitchingProtocols = 101,

        Status_OK                          = 200,
        Status_Created                     = 201,
        Status_Accepted                    = 202,
        Status_NonAuthoritativeInformation = 203,
        Status_NoContent                   = 204,
        Status_ResetContent                = 205,
        Status_PartialContent              = 206,

        Status_MultipleChoices   = 300,
        Status_MovedPermanently  = 301,
        Status_Found             = 302,
        Status_SeeOther          = 303,
        Status_NotModified       = 304,
        Status_UseProxy          = 305,
        Status_TemporaryRedirect = 307,

        Status_BadRequest                   = 400,
        Status_Unauthorized                 = 401,
        Status_PaymentRequired              = 402,
        Status_Forbidden                    = 403,
        Status_NotFound                     = 404,
        Status_MethodNotAllowed             = 405,
        Status_NotAcceptable                = 406,
        Status_ProxyAuthRequired            = 407,
        Status_RequestTimeout               = 408,
        Status_Conflict                     = 409,
        Status_Gone                         = 410,
        Status_LengthRequired               = 411,
        Status_PreconditionFailed           = 412,
        Status_RequestEntityTooLarge        = 413,
        Status_RequestURITooLong            = 414,
        Status_UnsupportedMediaType         = 415,
        Status_RequestedRangeNotSatisfiable = 416,
        Status_ExpectationFailed            = 417,

        Status_InternalServerError     = 500,
        Status_NotImplemented          = 501,
        Status_BadGateway              = 502,
        Status_ServiceUnavailable      = 503,
        Status_GatewayTimeout          = 504,
        Status_HTTPVersionNotSupported = 505,
    };

    void FillWithHtmlPage(StatusCode code,
                          const StringPiece& title = "",
                          const StringPiece& body = "");

public:
    HttpResponse() : m_status(Status_None) {}
    ~HttpResponse() {}
    virtual void Reset();

    StatusCode Status() const { return m_status; }
    void SetStatus(StatusCode status) {
        m_status = status;
    }
    static const char* StatusCodeToReasonPhrase(StatusCode status_code) {
        return InternalStatusCodeToReasonPhrase(status_code, NULL);
    }
    static const char* StatusCodeToReasonPhraseSafe(StatusCode status_code) {
        return InternalStatusCodeToReasonPhrase(status_code, "Unknown");
    }

    static const char* StatusCodeToDescription(StatusCode status_code);

    void Swap(HttpResponse* other) {
        HttpMessage::Swap(other);
        using std::swap;
        swap(m_status, other->m_status);
    }

private:
    static const char* InternalStatusCodeToReasonPhrase(
        StatusCode status_code, const char* no_match);
    virtual void AppendStartLineToString(std::string* result) const;
    virtual bool ParseStartLine(const StringPiece& data, ErrorCode* error);
    bool ParseStatusCode(StringPiece status);

    StatusCode m_status;
};

} // namespace toft

// adapt to std::swap
namespace std {

template <>
inline void swap(toft::HttpResponse& lhs, toft::HttpResponse& rhs) {
    lhs.Swap(&rhs);
}

} // namespace std

#endif // TOFT_NET_HTTP_RESPONSE_H
