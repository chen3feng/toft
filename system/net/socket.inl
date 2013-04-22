// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 03/23/12
// Description: inline functions of Socket classes

namespace toft {

//////////////////////////////////////////////////////////////////////////////
// Socket members

inline Socket::Socket() : m_handle(kInvalidHandle)
{
}

inline Socket::Socket(SOCKET handle) : m_handle(handle)
{
}

inline Socket::~Socket()
{
    Close();
}

inline SOCKET Socket::Handle() const
{
    return m_handle;
}

inline bool Socket::IsValid() const
{
    return m_handle != kInvalidHandle;
}

inline bool Socket::SetCloexec(bool value)
{
    return SetCloexec(static_cast<int>(m_handle), value);
}

inline bool Socket::GetOption(int level, int name, void* value, socklen_t* length) const
{
    return CheckError(getsockopt(m_handle, level, name, static_cast<char*>(value), length),
                      "GetOption");
}

inline bool Socket::SetOption(int level, int name, const void* value, socklen_t length)
{
    return CheckError(setsockopt(m_handle, level, name, static_cast<const char*>(value), length),
                      "SetOption");
}

inline bool Socket::GetOption(int level, int name, bool* value) const
{
    int int_value;
    bool result = GetOption(level, name, &int_value);
    *value = int_value != 0;
    return result;
}

inline bool Socket::SetOption(int level, int name, const bool& value)
{
    return SetOption(level, name, static_cast<int>(value));
}

inline bool Socket::GetError(int* error)
{
    return GetOption(SOL_SOCKET, SO_ERROR, error);
}

inline bool Socket::GetType(int* type) const
{
    return GetOption(SOL_SOCKET, SO_TYPE, type);
}

inline bool Socket::GetSendBufferSize(size_t* size) const
{
    return GetOption<size_t, int>(SOL_SOCKET, SO_SNDBUF, size);
}

inline bool Socket::SetSendBufferSize(size_t size)
{
    return SetOption<size_t, int>(SOL_SOCKET, SO_SNDBUF, size);
}

inline bool Socket::GetReceiveBufferSize(size_t* size) const
{
    return GetOption<size_t, int>(SOL_SOCKET, SO_RCVBUF, size);
}

inline bool Socket::SetReceiveBufferSize(size_t size)
{
    return SetOption<size_t, int>(SOL_SOCKET, SO_RCVBUF, size);
}

#ifdef _WIN32

inline bool Socket::SetSendTimeout(int seconds, int msec)
{
    int option = seconds * 1000 + msec;
    return SetOption(SOL_SOCKET, SO_SNDTIMEO, option);
}

inline bool Socket::SetReceiveTimeout(int seconds, int msec)
{
    int option = seconds * 1000 + msec;
    return SetOption(SOL_SOCKET, SO_RCVTIMEO, option);
}

inline bool Socket::SetSendTimeout(const timeval& tv)
{
    return SetSendTimeout(tv.tv_sec, tv.tv_usec / 1000);
}

inline bool Socket::SetReceiveTimeout(const timeval& tv)
{
    return SetReceiveTimeout(tv.tv_sec, tv.tv_usec / 1000);
}

#else // _WIN32

inline bool Socket::SetSendTimeout(const timeval& tv)
{
    return SetOption(SOL_SOCKET, SO_SNDTIMEO, tv);
}

inline bool Socket::SetReceiveTimeout(const timeval& tv)
{
    return SetOption(SOL_SOCKET, SO_RCVTIMEO, tv);
}

inline bool Socket::SetSendTimeout(int seconds, int msec)
{
    timeval tv = { seconds, msec * 1000 };
    return SetSendTimeout(tv);
}

inline bool Socket::SetReceiveTimeout(int seconds, int msec)
{
    timeval tv = { seconds, msec * 1000 };
    return SetReceiveTimeout(tv);
}

#endif // _WIN32

inline bool Socket::Ioctl(int cmd, int* value)
{
    return ioctlsocket(Handle(), cmd, reinterpret_cast<u_long*>(value)) == 0;
}

inline bool Socket::SetBlocking(bool value)
{
    return SocketSetNonblocking(Handle(), !value) == 0;
}

inline bool Socket::GetBlocking(bool* value)
{
    int n = SocketGetNonblocking(Handle(), value);
    *value = !(*value);
    return n == 0;
}

inline bool Socket::Bind(const SocketAddress& address)
{
    return CheckError(bind(Handle(), address.Address(), address.Length()), "Bind");
}

inline bool Socket::GetReuseAddress(bool* value)
{
    return GetOption(SOL_SOCKET, SO_REUSEADDR, value);
}

inline bool Socket::SetReuseAddress(bool value)
{
    return SetOption(SOL_SOCKET, SO_REUSEADDR, value);
}

inline bool Socket::SetKeepAlive(bool onoff)
{
    return SetOption(SOL_SOCKET, SO_KEEPALIVE, onoff);
}

inline bool Socket::GetKeepAlive(bool* onoff)
{
    return GetOption(SOL_SOCKET, SO_KEEPALIVE, onoff);
}

inline bool Socket::SetTcpNoDelay(bool onoff)
{
    return SetOption(IPPROTO_TCP, TCP_NODELAY, onoff);
}

inline bool Socket::GetTcpNoDelay(bool* onoff)
{
    return GetOption(IPPROTO_TCP, TCP_NODELAY, onoff);
}

inline bool Socket::IsReadable()
{
    struct timeval tv = {0, 0};
    return WaitReadable(&tv);
}

inline bool Socket::IsWriteable()
{
    struct timeval tv = {0, 0};
    return WaitWriteable(&tv);
}

inline int Socket::GetLastError()
{
    return SocketGetLastError();
}

inline std::string Socket::GetErrorString(int error)
{
    return SocketGetErrorString(error);
}

inline std::string Socket::GetLastErrorString()
{
    return SocketGetErrorString(GetLastError());
}

inline void Socket::SetLastError(int error)
{
    SocketSetLastError(error);
}

inline void Socket::VerifyHandle(int fd)
{
    assert(fd != kInvalidHandle);
}

inline bool Socket::IsInterruptedAndRestart(bool restart)
{
    return restart && GetLastError() == SOCKET_ERROR_CODE(EINTR);
}

/////////////////////////////////////////////////////////////////////////////
// ListenerSocket members

inline ListenerSocket::ListenerSocket()
{
}

inline ListenerSocket::ListenerSocket(int af, int type, int protocol) :
    Socket(socket(af, type, protocol))
{
}

inline bool ListenerSocket::Listen(int backlog)
{
    return CheckError(listen(Handle(), backlog), "Listen");
}

/////////////////////////////////////////////////////////////////////////////
// StreamSocket members

inline StreamSocket::StreamSocket(int af, int protocol)
{
    if (!DataSocket::Create(af, SOCK_STREAM, protocol))
        throw SocketError("StreamSocket");
}

inline bool StreamSocket::Create(sa_family_t af, int protocol)
{
    return Socket::Create(af, SOCK_STREAM, protocol);
}

inline bool StreamSocket::Shutdown()
{
    return CheckError(SocketShutdown(Handle()), "Shutdown");
}

inline bool StreamSocket::ShutdownSend()
{
    return CheckError(SocketShutdownSend(Handle()), "ShutdownSend");
}

inline bool StreamSocket::ShutdownReceive()
{
    return CheckError(SocketShutdownReceive(Handle()), "ShutdownReceive");
}

/////////////////////////////////////////////////////////////////////////////
// DatagramSocket members

inline bool DatagramSocket::Create(int af, int protocol)
{
    return Socket::Create(af, SOCK_DGRAM, protocol);
}
} // namespace toft

