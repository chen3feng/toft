// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_NET_HTTP_HTTP_SERVER_H
#define TOFT_NET_HTTP_HTTP_SERVER_H
#pragma once

#include <string>

namespace toft {

class HttpHandler;

class HttpServer {
public:
    HttpServer();
    virtual ~HttpServer();
    void RegisterHttpHandler(const std::string& path, HttpHandler* handler);
private:
};

} // namespace toft

#endif // TOFT_NET_HTTP_HTTP_SERVER_H
