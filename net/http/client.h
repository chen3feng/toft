// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: DongPing HUANG <hdping99@gmail.com>
// Created: 11/05/11

#ifndef TOFT_NET_HTTP_CLIENT_H
#define TOFT_NET_HTTP_CLIENT_H
#pragma once

#include <string>
#include <vector>

#include "toft/net/http/request.h"
#include "toft/net/http/response.h"
#include "toft/system/net/socket.h"

namespace toft {

// Helper class to download a page. Support GET/POST methods.
// Now only the easiest case is supported. In the future, we need to support
// some more complicated cases, for example, forward, encoding, response in
// stream mode( which is neccessary to download huge file.), etc.
class HttpClient {
public:
    enum ErrorCode {
        SUCCESS = 0,
        ERROR_INVALID_URI_ADDRESS,
        ERROR_INVALID_PROXY_ADDRESS,
        ERROR_INVALID_RESPONSE_HEADER,
        ERROR_FAIL_TO_RESOLVE_ADDRESS,
        ERROR_FAIL_TO_SEND_REQUEST,
        ERROR_FAIL_TO_GET_RESPONSE,
        ERROR_FAIL_TO_CONNECT_SERVER,
        ERROR_FAIL_TO_READ_CHUNKSIZE,
        ERROR_PROTOCAL_NOT_SUPPORTED,
        ERROR_CONTENT_TYPE_NOT_SUPPORTED,
        ERROR_HTTP_STATUS_CODE, // such as HTTP 404
        ERROR_TOO_MANY_REDIRECTS, // TODO(chen3feng): support redirection
    };

    // query error message from error code
    static const char* GetErrorMessage(ErrorCode error_code);

public:
    // Per-request options
    class Options {
    public:
        Options() : m_encoding(""), m_max_response_length(0) {}
        Options& SetAcceptLanguage(const std::string& languages);
        const std::string& AccpetLanguage() const;
        Options& AddHeader(const std::string& name, const std::string& value);
        const HttpHeaders& Headers() const;
        Options& SetMaxResponseLength(size_t length);
        size_t MaxResponseLength() const;
    private:
        std::string m_encoding;
        HttpHeaders m_headers;
        size_t m_max_response_length;
    };

public:
    HttpClient();
    ~HttpClient();

    HttpClient& SetProxy(const std::string& proxy);
    const std::string& Proxy() const;

    HttpClient& SetUserAgent(const std::string& user_agent);
    const std::string& UserAgent() const;

    size_t GetMaxResponseLength() const;

    // Request url with GET method, output stored into response object.
    bool Get(const std::string& url,
             HttpResponse* response,
             ErrorCode* error = NULL);

    // Request url with GET method, output stored into response object.
    // Some options supported, for example, http socket proxy.
    bool Get(const std::string& url,
             const Options& options,
             HttpResponse* response,
             ErrorCode* error = NULL);

    // Request url with POST method, output stored into response object.
    bool Post(const std::string& url,
              const std::string& data,
              HttpResponse* response,
              ErrorCode* error = NULL);

    // Request url with POST method, output stored into response object.
    // Some options supported, for example, http socket proxy.
    bool Post(const std::string& url,
              const std::string& data,
              const Options& options,
              HttpResponse* response,
              ErrorCode* error = NULL);

    // Request url with PUT method, output stored into response object.
    bool Put(const std::string& url,
             const std::string& data,
             HttpResponse* response,
             ErrorCode* error = NULL);

    // Request url with PUT method, output stored into response object.
    // Some options supported, for example, http socket proxy.
    bool Put(const std::string& url,
             const std::string& data,
             const Options& options,
             HttpResponse* response,
             ErrorCode* error = NULL);

    // Request url with DELETE method, output stored into response object.
    bool Delete(const std::string& url,
                HttpResponse* response,
                ErrorCode* error = NULL);

    // Request url with DELETE method, output stored into response object.
    // Some options supported, for example, http socket proxy.
    bool Delete(const std::string& url,
                const Options& options,
                HttpResponse* response,
                ErrorCode* error = NULL);

private:
    bool Request(HttpRequest::MethodType method,
                 const std::string& url,
                 const std::string& data,
                 const Options& options,
                 HttpResponse *response,
                 ErrorCode *error);

private:
    std::string m_proxy;
    std::string m_user_agent;
};

} // namespace toft

#endif // TOFT_NET_HTTP_CLIENT_H
