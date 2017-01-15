// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 09/30/11
// Description: Socket classes implementation

#include "toft/system/net/socket.h"

#include "toft/system/eintr_ignored.h"

namespace toft {

/////////////////////////////////////////////////////////////////////////////
// Socket class members

SocketError::SocketError(const char* info, int error) :
    std::runtime_error(std::string(info) + std::string(": ") + SocketGetErrorString(error)),
    m_error(error)
{
}

/////////////////////////////////////////////////////////////////////////////
// Socket class members

void Socket::ReportError(const char* info) const
{
}

void Socket::Attach(SOCKET handle)
{
    if (handle != m_handle)
    {
        closesocket(m_handle);
        m_handle = handle;
    }
}

SOCKET Socket::Detach()
{
    SOCKET old = m_handle;
    m_handle = kInvalidHandle;
    return old;
}

bool Socket::Create(int af, int type, int protocol)
{
    Close();
    m_handle = socket(af, type, protocol);
    if (IsValid())
    {
        return true;
    }
    else
    {
        ReportError("Create");
        return false;
    }
}

bool Socket::Close()
{
    if (IsValid())
    {
        SOCKET fd = m_handle;
        m_handle = kInvalidHandle;
        return CheckError(closesocket(fd), "Close");
    }
    return false;
}

/* Set the FD_CLOEXEC flag of desc if value is nonzero,
   or clear the flag if value is 0.
   Return 0 on success, or -1 on error with errno set. */

bool Socket::SetCloexec(int desc, bool value)
{
#ifdef __unix__
    int oldflags = fcntl(desc, F_GETFD, 0);
    if (oldflags < 0)
        return false;

    if (value)
        oldflags |= FD_CLOEXEC;
    else
        oldflags &= ~FD_CLOEXEC;

    return fcntl(desc, F_SETFD, oldflags) >= 0;
#else
    return true;
#endif
}


bool Socket::GetLocalAddress(SocketAddress* address) const
{
    socklen_t length = address->Capacity();
    if (CheckError(getsockname(m_handle, address->Address(), &length), "GetLocalAddress"))
    {
        address->SetLength(length);
        return true;
    }
    return false;
}

bool Socket::GetPeerAddress(SocketAddress* address) const
{
    socklen_t length = address->Capacity();
    if (CheckError(getpeername(m_handle, address->Address(), &length), "GetPeerAddress"))
    {
        address->SetLength(length);
        return true;
    }
    return false;
}

bool Socket::SetLinger(bool onoff, int timeout)
{
    struct linger l;
    l.l_onoff = onoff;
    l.l_linger = (u_short) timeout;
    return SetOption(SOL_SOCKET, SO_LINGER, l);
}

#if __unix__
bool Socket::SetTcpKeepAliveOption(int idle, int interval, int count)
{
    return
        SetOption(SOL_SOCKET, SO_KEEPALIVE, 1) &&
        SetOption(SOL_TCP, TCP_KEEPIDLE, idle) &&
        SetOption(SOL_TCP, TCP_KEEPINTVL, interval) &&
        SetOption(SOL_TCP, TCP_KEEPCNT, count);
}
#endif

bool Socket::WaitReadable(struct timeval* tv, bool restart)
{
    for (;;)
    {
        int n = Socket_PollReadable(Handle(), tv);
        if (n != SOCKET_ERROR_RETURN)
        {
            return n > 0;
        }
        else if (!IsInterruptedAndRestart(restart))
        {
            CheckError(n, "WaitReadable");
            break;
        }
    }
    return false;
}

bool Socket::WaitWriteable(struct timeval* tv, bool restart)
{
    for (;;)
    {
        int n = Socket_PollWriteable(Handle(), tv);
        if (n != SOCKET_ERROR_RETURN)
        {
            return n > 0;
        }
        else if (!IsInterruptedAndRestart(restart))
        {
            CheckError(n, "WaitWriteable");
            break;
        }
    }
    return false;
}

bool Socket::CheckError(int result, const char* info) const
{
    if (result != 0)
    {
        ReportError(info);
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// ListenerSocket members

ListenerSocket::ListenerSocket(const SocketAddress& address, int type) :
    Socket(socket(address.Family(), type, 0))
{
    if (!IsValid())
        throw SocketError("ListenerSocket");
    if (!Bind(address))
    {
        throw std::runtime_error("Can't bind to " + address.ToString());
    }
}

bool ListenerSocket::Accept(Socket* socket, bool auto_restart)
{
    return Accept(socket, NULL, auto_restart);
}

bool ListenerSocket::Accept(
    Socket* socket,
    SocketAddress* address,
    bool auto_restart)
{
    socklen_t length = sizeof(*address);
    for (;;)
    {
        SOCKET s = accept(Handle(), address ? address->Address() : NULL, &length);
        if (s != kInvalidHandle)
        {
            socket->Attach(s);
            if (address)
                address->SetLength(length);
            return true;
        }
        else
        {
            if (!auto_restart || GetLastError() != SOCKET_ERROR_CODE(EINTR))
            {
                ReportError("Accept");
                break;
            }
        }
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
// DataSocket members

bool DataSocket::Connect(const SocketAddress& address)
{
    if (connect(Handle(), address.Address(), address.Length()) != 0)
    {
        switch (errno)
        {
        case SOCKET_ERROR_CODE(EINTR):
        case SOCKET_ERROR_CODE(EWOULDBLOCK):
            return true;
        case SOCKET_ERROR_CODE(EINPROGRESS):
            {
                bool blocking = true;
                if (GetBlocking(&blocking) && !blocking)
                    return true;
            }
        }
        ReportError("Connect");
        return false;
    }
    return true;
}

bool DataSocket::Connect(const SocketAddress& address, int64_t timeout_ms)
{
    if (timeout_ms < 0)
        return Connect(address);

    bool init_block = false;
    if (!GetBlocking(&init_block))
        return false;

    if (init_block)
    {
        if (!SetBlocking(false))
            return false;
    }

    int rv = TOFT_EINTR_IGNORED(connect(Handle(), address.Address(), address.Length()));

    bool ret = false;
    if (rv == 0) // Connected immediately
    {
        ret = true;
    }
    else
    {
        int error = errno;
        switch (error)
        {
        case EWOULDBLOCK: // For winsock
        case EINPROGRESS:
            {
                timeval tv = {
                    static_cast<time_t>(timeout_ms / 1000),
                    static_cast<suseconds_t>(timeout_ms % 1000 * 1000) };
                if (WaitWriteable(&tv))
                {
                    // WaitWriteable success doesn't means connect success.
                    // So must check the internal error code of socket.
                    int error = 0;
                    // TODO(phongchen): GetError will clear the error code,
                    // this make user code can't obtain the reason, I don't know
                    // how to deal this problem.
                    if (GetError(&error) && error == 0)
                        ret = true;
                }
            }
        }
    }

    if (init_block)
        SetBlocking(true);

    if (!ret)
        ReportError("Connect");

    return ret;
}

bool DataSocket::Send(
    const void* buffer,
    size_t buffer_size,
    size_t* sent_length,
    int flags,
    bool auto_restart)
{
    for (;;)
    {
        int n = send(Handle(), (const char*)buffer, buffer_size, flags);
        if (n != SOCKET_ERROR_RETURN)
        {
            *sent_length = n;
            return true;
        }
        else
        {
            if (!IsInterruptedAndRestart(auto_restart))
            {
                *sent_length = 0;
                ReportError("Send");
                return false;
            }
        }
    }
}

bool DataSocket::Receive(
    void* buffer,
    size_t buffer_size,
    size_t* received_size,
    int flags,
    bool auto_restart)
{
    for (;;)
    {
        int n = recv(Handle(), static_cast<char*>(buffer), buffer_size, flags);
        if (n != SOCKET_ERROR_RETURN)
        {
            *received_size = n;
            break;
        }
        else if (!IsInterruptedAndRestart(auto_restart))
        {
            *received_size = 0;
            ReportError("Receive");
            break;
        }
    }
    return *received_size > 0;
}

bool DataSocket::Receive(
    void* buffer,
    size_t buffer_size,
    size_t* received_size,
    timeval* timeout,
    int flags,
    bool auto_restart)
{
    if (WaitReadable(timeout, auto_restart))
        return Receive(buffer, buffer_size, received_size, flags, auto_restart);
    else
        return false;
}

/////////////////////////////////////////////////////////////////////////////
// StreamSocket members

bool StreamSocket::ReceiveAll(
    void *buffer,
    size_t buffer_size,
    size_t* received_size,
    int flags,
    bool auto_restart)
{
    assert((flags & MSG_PEEK) == 0);
#ifdef MSG_WAITALL
    flags |= MSG_WAITALL;
#endif
    *received_size = 0;
    while (buffer_size > 0)
    {
        size_t n;
        if (Receive(buffer, buffer_size, &n, flags, auto_restart))
        {
            if (n == 0)
            {
                SetLastError(SOCKET_ERROR_CODE(ECONNRESET));
                return false;
            }
            buffer = static_cast<char*>(buffer) + n;
            buffer_size -= n;
            *received_size += n;
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool StreamSocket::ReceiveAll(
    void *buffer,
    size_t buffer_size,
    int flags,
    bool auto_restart)
{
    size_t received_size;
    return ReceiveAll(buffer, buffer_size, &received_size, flags, auto_restart);
}

bool StreamSocket::ReceiveAll(
    void *buffer,
    size_t buffer_size,
    size_t* received_size,
    timeval* timeout,
    int flags,
    bool auto_restart)
{
    assert((flags & MSG_PEEK) == 0);
#ifdef MSG_WAITALL
    flags |= MSG_WAITALL;
#endif
    *received_size = 0;
    while (buffer_size > 0)
    {
        if (WaitReadable(timeout, auto_restart))
        {
            size_t n;
            if (Receive(buffer, buffer_size, &n, flags, auto_restart))
            {
                if (n == 0)
                {
                    SetLastError(SOCKET_ERROR_CODE(ECONNRESET));
                    return false;
                }
                buffer = static_cast<char*>(buffer) + n;
                buffer_size -= n;
                *received_size += n;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool StreamSocket::ReceiveAll(
    void *buffer,
    size_t buffer_size,
    timeval* timeout,
    int flags,
    bool auto_restart)
{
    size_t received_size;
    return ReceiveAll(buffer, buffer_size, &received_size, timeout, flags, auto_restart);
}

bool StreamSocket::ReceiveLine(
    void* buffer,
    size_t buffer_size,
    size_t* received_size,
    size_t max_peek_size)
{
    *received_size = 0;

    while (buffer_size > 0)
    {
        size_t peek_size = buffer_size > max_peek_size ? max_peek_size : buffer_size;
        size_t n;
        if (Receive(buffer, peek_size, &n, MSG_PEEK) && n > 0)
        {
            char* p = static_cast<char*>(memchr(buffer, '\n', n));
            if (p)
            {
                bool result = ReceiveAll(buffer, p - static_cast<char*>(buffer) + 1, &n);
                *received_size += n;
                return result;
            }
            else
            {
                bool result = ReceiveAll(buffer, n, &n);
                *received_size += n;
                if (!result)
                    return false;
                buffer = static_cast<char*>(buffer) + n;
                buffer_size -= n;
            }
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool StreamSocket::ReceiveLine(std::string* str, size_t peek_size)
{
    const size_t kMaxPeekSize = 1024;
    char buffer[kMaxPeekSize];
    peek_size = peek_size > kMaxPeekSize ? kMaxPeekSize : peek_size;

    str->clear();

    for (;;)
    {
        size_t n;
        if (Receive(buffer, peek_size, &n, MSG_PEEK) && n > 0)
        {
            char* p = static_cast<char*>(memchr(buffer, '\n', n));
            if (p)
            {
                bool result = ReceiveAll(buffer, p - buffer + 1, &n);
                str->append(buffer, n);
                return result;
            }
            else
            {
                bool result = ReceiveAll(buffer, n, &n);
                str->append(buffer, n);
                if (!result)
                    return false;
            }
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool StreamSocket::SendAll(
    const void* buffer,
    size_t buffer_size,
    size_t* sent_size,
    int flags,
    bool auto_restart)
{
    *sent_size = 0;
    while (buffer_size > 0)
    {
        size_t current_sent_size;
        if (Send(buffer, buffer_size, &current_sent_size, flags, auto_restart))
        {
            buffer = (const char*)buffer + current_sent_size;
            buffer_size -= current_sent_size;
            *sent_size += current_sent_size;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool StreamSocket::SendAll(
    const void* buffer,
    size_t buffer_size,
    int flags,
    bool auto_restart)
{
    size_t sent_size;
    return SendAll(buffer, buffer_size, &sent_size, flags, auto_restart);
}

bool StreamSocket::SendAll(
    const void* buffer,
    size_t buffer_size,
    size_t* sent_size,
    timeval* tv,
    int flags,
    bool auto_restart)
{
    *sent_size = 0;
    while (buffer_size > 0)
    {
        if (WaitWriteable(tv, auto_restart))
        {
            size_t current_sent_size;
            if (Send(buffer, buffer_size, &current_sent_size, flags, auto_restart))
            {
                buffer = (const char*)buffer + current_sent_size;
                buffer_size -= current_sent_size;
                *sent_size += current_sent_size;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// DatagramSocket members

DatagramSocket::DatagramSocket(int af, int protocol)
{
    if (!Create(af, protocol))
        throw SocketError("DatagramSocket");
}

bool DatagramSocket::SendTo(
    const void* buffer,
    size_t buffer_size,
    const SocketAddress& address,
    size_t* sent_size)
{
    int n = sendto(
        Handle(), (const char*)buffer, buffer_size, 0,
        address.Address(), address.Length()
    );
    if (n >= 0)
    {
        *sent_size = n;
        return true;
    }
    else
    {
        *sent_size = 0;
        ReportError("SendTo");
        return false;
    }
}

bool DatagramSocket::ReceiveFrom(
    void* buffer,
    size_t buffer_size,
    size_t* received_size,
    SocketAddress* address,
    int flags)
{
    socklen_t address_length = address->Capacity();
    int result = recvfrom(
        Handle(),
        static_cast<char*>(buffer), buffer_size, flags,
        address->Address(), &address_length
    );
    if (result >= 0)
    {
        *received_size = result;
        address->SetLength(address_length);
        return true;
    }
    else
    {
        *received_size = 0;
        ReportError("ReceiveFrom");
    }
    return false;
}

} // namespace toft

