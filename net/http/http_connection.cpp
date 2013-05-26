// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/net/http/http_connection.h"
#include "thirdparty/glog/logging.h"

namespace toft {

HttpConnection::HttpConnection(EventDispatcher* dispatcher, int fd)
    : m_watcher(dispatcher, std::bind(&HttpConnection::OnIoEvents, this,
                                      std::placeholders::_1),
                fd, EventMask_Read),
      m_received_size(0),
      m_sent_size(0) {
    m_socket.Attach(fd);
    m_watcher.Start();
}

void HttpConnection::Send(const StringPiece& data) {
}

void HttpConnection::Close() {
}

void HttpConnection::OnIoEvents(int events) {
    LOG(INFO) << "HttpConnection::OnIoEvents";
    if (events & EventMask_Error) {
        LOG(INFO) << "Error";
        m_watcher.Stop();
        return;
    }
    if (events & EventMask_Read) {
        LOG(INFO) << "Read";
        OnReadable();
    }
    if (events & EventMask_Write) {
        LOG(INFO) << "Write";
        OnWriteable();
    }
    int new_events = EventMask_Read;
    if (!m_send_queue.empty())
        new_events |= EventMask_Write;
    m_watcher.Set(new_events);
}

void HttpConnection::OnReadable() {
    size_t kBufferSize = 65536;
    m_receive_buffer.resize(m_received_size + kBufferSize);
    size_t received_size;
    if (m_socket.Receive(&m_receive_buffer[m_received_size], kBufferSize,
                         &received_size)) {
        m_received_size += received_size;
    } else {
        OnClosed();
    }
}

void HttpConnection::OnWriteable() {
    m_watcher.Stop();
}

void HttpConnection::OnClosed() {
    int events = m_send_queue.empty() ? EventMask_None : EventMask_Write;
    m_watcher.Set(events);
}

} // namespace toft
