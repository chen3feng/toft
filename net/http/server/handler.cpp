// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/net/http/server/handler.h"
#include "toft/base/string/number.h"

namespace toft {

void HttpHandler::HandleRequest(const HttpRequest* req, HttpResponse* resp) {
    switch (req->Method()) {
    case HttpRequest::METHOD_HEAD:
        HandleHead(req, resp);
        break;
    case HttpRequest::METHOD_GET:
        HandleGet(req, resp);
        break;
    case HttpRequest::METHOD_POST:
        HandlePost(req, resp);
        break;
    case HttpRequest::METHOD_PUT:
        HandlePut(req, resp);
        break;
    case HttpRequest::METHOD_DELETE:
        HandleDelete(req, resp);
        break;
    case HttpRequest::METHOD_OPTIONS:
        HandleOptions(req, resp);
        break;
    case HttpRequest::METHOD_TRACE:
        HandleTrace(req, resp);
        break;
    case HttpRequest::METHOD_CONNECT:
        HandleConnect(req, resp);
        break;
    default:
        MethodNotAllowed(req, resp);
        break;
    }
}

void HttpHandler::HandleHead(const HttpRequest* req, HttpResponse* resp) {
    MethodNotAllowed(req, resp);
}

void HttpHandler::HandleGet(const HttpRequest* req, HttpResponse* resp) {
    MethodNotAllowed(req, resp);
}

void HttpHandler::HandlePost(const HttpRequest* req, HttpResponse* resp) {
    MethodNotAllowed(req, resp);
}

void HttpHandler::HandlePut(const HttpRequest* req, HttpResponse* resp) {
    MethodNotAllowed(req, resp);
}

void HttpHandler::HandleDelete(const HttpRequest* req, HttpResponse* resp) {
    MethodNotAllowed(req, resp);
}

void HttpHandler::HandleOptions(const HttpRequest* req, HttpResponse* resp) {
    MethodNotAllowed(req, resp);
}

void HttpHandler::HandleTrace(const HttpRequest* req, HttpResponse* resp) {
    MethodNotAllowed(req, resp);
}

void HttpHandler::HandleConnect(const HttpRequest* req, HttpResponse* resp) {
    MethodNotAllowed(req, resp);
}

void HttpHandler::MethodNotAllowed(const HttpRequest* req, HttpResponse* resp) {
    resp->SetStatus(HttpResponse::Status_MethodNotAllowed);
    resp->SetBody("Method Not Allowed");
    resp->SetHeader("Content-Length", NumberToString(resp->Body().size()));
}

} // namespace toft

