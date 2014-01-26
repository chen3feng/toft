// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/net/http/server/connection.h"
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
    m_send_queue.push_back(data.as_string());
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
        if (!OnReadable())
            return;
    }
    if (events & EventMask_Write) {
        LOG(INFO) << "Write";
        if (!OnWriteable())
            return;
    }

    int new_events = EventMask_Read;
    if (!m_send_queue.empty())
        new_events |= EventMask_Write;
    m_watcher.Set(new_events);
}

bool HttpConnection::OnReadable() {
    size_t kBufferSize = 65536;
    m_receive_buffer.resize(m_received_size + kBufferSize);
    size_t received_size;
    char* buf = &m_receive_buffer[m_received_size];
    if (!m_socket.Receive(buf, kBufferSize, &received_size)) {
        OnClosed();
        return false;
    }
    m_received_size += received_size;
    LOG(INFO) << std::string(m_receive_buffer.data(), m_receive_buffer.size());
    HttpRequest request;

    return true;
}

bool HttpConnection::OnWriteable() {
    while (!m_send_queue.empty()) {
        const std::string& data = m_send_queue.front();
        size_t data_size = data.size() - m_sent_size;
        size_t sent_size;
        if (m_socket.Send(data.data() + m_sent_size, data_size, &sent_size)) {
            if (sent_size == data_size) {
                m_send_queue.pop_front();
                m_sent_size = 0;
            } else {
                m_sent_size += sent_size;
                break;
            }
        } else {
            break;
        }
    }
    return true;
}

void HttpConnection::OnClosed() {
    m_watcher.Stop();
}

} // namespace toft
