// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_NET_HTTP_HTTP_CONNECTION_H
#define TOFT_NET_HTTP_HTTP_CONNECTION_H
#pragma once

namespace toft {

class HttpConnection {
public:
    void Send(const StringPiece& data);
    void Close();
private:
    virtual void OnReceive(const StringPiece& data);
    virtual void OnClose();
};

} // namespace toft

#endif // TOFT_NET_HTTP_HTTP_CONNECTION_H
