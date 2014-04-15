// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/net/http/server/server.h"
#include <set>
#include "toft/net/http/server/connection.h"
#include "toft/system/event_dispatcher/event_dispatcher.h"
#include "toft/system/net/socket.h"

#include "thirdparty/glog/logging.h"

namespace toft {

struct HttpServer::Impl {
    Impl()
        : m_listen_socket(AF_INET, SOCK_STREAM, 0),
          m_listen_watcher(&m_event_dispatcher,
                           std::bind(&Impl::OnAccept, this,
                                     std::placeholders::_1)) {
        m_listen_socket.SetBlocking(false);
    }

public:
    bool Bind(const SocketAddress& address, SocketAddress* real_address) {
        if (!m_listen_socket.Bind(address))
            return false;
        if (real_address)
            return m_listen_socket.GetLocalAddress(real_address);
        return true;
    }

    bool Start() {
        if (!m_listen_socket.Listen()) {
            return false;
        }
        m_listen_watcher.Set(m_listen_socket.Handle(), EventMask_Write);
        m_listen_watcher.Start();
        return true;
    }

    bool RegisterHttpHandler(const std::string& path, HttpHandler* handler) {
        return m_handler_map.insert(std::make_pair(path, handler)).second;
    }

    void Run() {
        m_event_dispatcher.Run();
    }

private:
    void OnAccept(int events) {
        StreamSocket socket;
        SocketAddressStorage address;
        if (m_listen_socket.Accept(&socket, &address)) {
            LOG(INFO) << "Connect from " << address.ToString() << " acceptted.";
            socket.SetBlocking(false);
            m_connections.insert(new HttpConnection(&m_event_dispatcher,
                                                    socket.Detach()));
        }
    }

private:
    std::map<std::string, HttpHandler*> m_handler_map;
    EventDispatcher m_event_dispatcher;
    ListenerSocket m_listen_socket;
    IoEventWatcher m_listen_watcher;
    std::set<HttpConnection*> m_connections;
};

HttpServer::HttpServer() : m_impl(new Impl()) {
}

HttpServer::~HttpServer() {
}

bool HttpServer::RegisterHttpHandler(const std::string& path,
                                     HttpHandler* handler) {
    return m_impl->RegisterHttpHandler(path, handler);
}

bool HttpServer::Bind(const SocketAddress& address, SocketAddress* real_address) {
    return m_impl->Bind(address, real_address);
}

bool HttpServer::Start() {
    return m_impl->Start();
}

void HttpServer::Run() {
    return m_impl->Run();
}

} // namespace toft
