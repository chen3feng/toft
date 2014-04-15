// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_NET_HTTP_SERVER_SERVER_H
#define TOFT_NET_HTTP_SERVER_SERVER_H
#pragma once

#include <map>
#include <string>
#include "toft/base/scoped_ptr.h"
#include "toft/base/uncopyable.h"
#include "toft/system/net/socket_address.h"

namespace toft {

class HttpHandler;

class HttpServer {
    TOFT_DECLARE_UNCOPYABLE(HttpServer);

public:
    HttpServer();
    virtual ~HttpServer();
    bool RegisterHttpHandler(const std::string& path, HttpHandler* handler);
    bool Bind(const SocketAddress& address, SocketAddress* real_address = NULL);
    bool Start();
    void Close();
    void Run();

private:
    struct Impl;
    scoped_ptr<Impl> m_impl;
};

} // namespace toft

#endif // TOFT_NET_HTTP_SERVER_SERVER_H
