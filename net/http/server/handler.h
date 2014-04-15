// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_NET_HTTP_SERVER_HANDLER_H
#define TOFT_NET_HTTP_SERVER_HANDLER_H
#pragma once

#include "toft/net/http/request.h"
#include "toft/net/http/response.h"

namespace toft {

class HttpHandler {
private:
    HttpHandler();
public:
    virtual ~HttpHandler() {}
    virtual void HandleRequest(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleGet(const HttpRequest* req, HttpResponse* resp);
    virtual void HandlePost(const HttpRequest* req, HttpResponse* resp);
    virtual void HandlePut(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleHead(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleDelete(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleOptions(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleTrace(const HttpRequest* req, HttpResponse* resp);
    virtual void HandleConnect(const HttpRequest* req, HttpResponse* resp);
protected:
    void MethodNotAllowed(const HttpRequest* req, HttpResponse* resp);
private:
};

} // namespace toft

#endif // TOFT_NET_HTTP_SERVER_HANDLER_H
