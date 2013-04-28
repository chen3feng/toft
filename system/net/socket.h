// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_NET_SOCKET_H
#define TOFT_SYSTEM_NET_SOCKET_H

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <stdexcept>
#include <string>
#include <utility>

#include "toft/system/net/os_socket.h"
#include "toft/system/net/socket_address.h"

namespace toft {

/// socket error exception class
class SocketError : public std::runtime_error
{
public:
    SocketError(const char* info = "socket error", int error = SocketGetLastError());
    int Code() const { return m_error; }
private:
    int m_error;
};

/// Abstract socket base class
class Socket
{
public:
    static const SOCKET kInvalidHandle = INVALID_SOCKET_HANDLE;

protected:
    Socket();
    explicit Socket(SOCKET handle);
    bool Create(int af, int type, int protocol = 0);

public:
    virtual ~Socket();
    SOCKET Handle() const;
    bool IsValid() const;

    /// Attach a socket handle to this object
    void Attach(SOCKET handle);

    /// Detach socket handle from this object
    SOCKET Detach();

    bool Close();

    /// Set the FD_CLOEXEC flag of desc if value is nonzero,
    /// or clear the flag if value is 0.
    /// Return 0 on success, or -1 on error with errno set.
    static bool SetCloexec(int desc, bool value);
    bool SetCloexec(bool value = true);

    bool GetOption(int level, int name, void* value, socklen_t* length) const;
    bool SetOption(int level, int name, const void* value, socklen_t length);

    template <typename T>
    bool GetOption(int level, int name, T* value) const
    {
        socklen_t length = sizeof(value);
        return GetOption(level, name, value, &length);
    }

    template <typename T>
    bool SetOption(int level, int name, const T& value)
    {
        socklen_t length = sizeof(value);
        return SetOption(level, name, &value, length);
    }

    /// Get socket option with difference type
    template <typename Type, typename InternalType>
    bool GetOption(int level, int name, Type* value) const
    {
        InternalType internal_value;
        bool result = GetOption(level, name, &internal_value);
        *value = static_cast<Type>(internal_value);
        return result;
    }

    /// Set socket option with difference type
    template <typename Type, typename InternalType>
    bool SetOption(int level, int name, const Type& value)
    {
        return SetOption(level, name, static_cast<InternalType>(value));
    }

    bool GetOption(int level, int name, bool* value) const;
    bool SetOption(int level, int name, const bool& value);

    bool GetError(int* error);
    bool GetType(int* type) const;

    bool GetSendBufferSize(size_t* size) const;
    bool SetSendBufferSize(size_t size);
    bool GetReceiveBufferSize(size_t* size) const;
    bool SetReceiveBufferSize(size_t size);

#ifdef _WIN32
    bool SetSendTimeout(int seconds, int msec = 0);
    bool SetSendTimeout(const timeval& tv);
    bool SetReceiveTimeout(int seconds, int msec = 0);
    bool SetReceiveTimeout(const timeval& tv);
#else
    bool SetSendTimeout(const timeval& tv);
    bool SetSendTimeout(int seconds, int msec = 0);
    bool SetReceiveTimeout(const timeval& tv);
    bool SetReceiveTimeout(int seconds, int msec = 0);
#endif
    bool Ioctl(int cmd, int* value);

    bool SetBlocking(bool value = true);
    bool GetBlocking(bool* value);

    bool Bind(const SocketAddress& address);

    bool GetLocalAddress(SocketAddress* address) const;

    bool GetPeerAddress(SocketAddress* address) const;

    bool GetReuseAddress(bool* value);
    bool SetReuseAddress(bool value = true);

    bool SetLinger(bool onoff = true, int timeout = 0);
    bool SetKeepAlive(bool onoff = true);

    bool GetKeepAlive(bool* onoff);

#if __unix__
    bool SetTcpKeepAliveOption(int idle, int interval, int count);
#endif
    bool SetTcpNoDelay(bool onoff = true);
    bool GetTcpNoDelay(bool* onoff);

    bool WaitReadable(struct timeval* tv = NULL, bool restart = true);
    bool WaitWriteable(struct timeval* tv = NULL, bool restart = true);

    bool IsReadable();
    bool IsWriteable();

public:
    static int GetLastError();
    static std::string GetErrorString(int error);
    static std::string GetLastErrorString();

protected:
    bool CheckError(int result, const char* info = "socket") const;
    void ReportError(const char* info) const;
    static void SetLastError(int error);
    static void VerifyHandle(int fd);
    static bool IsInterruptedAndRestart(bool restart);

private:
    Socket(const Socket&);
    Socket& operator=(const Socket&);

private:
    SOCKET m_handle;
};

/// Listen streaming connections from client
class ListenerSocket : public Socket
{
public:
    ListenerSocket();
    ListenerSocket(int af, int type, int protocol);
    ListenerSocket(const SocketAddress& address, int type = SOCK_STREAM);
    using Socket::Create;
    bool Listen(int backlog = SOMAXCONN);

    bool Accept(Socket* socket, bool auto_restart = true);
    bool Accept(Socket* socket, SocketAddress* address, bool auto_restart = true);
};

/// Abstract data transfer socket
class DataSocket : public Socket
{
protected:
    DataSocket() {}

public:
    bool Connect(const SocketAddress& address);

    /// Connect with timeout
    bool Connect(const SocketAddress& address, int64_t timeout_ms);

    /// Send data
    bool Send(
        const void* buffer,
        size_t buffer_size,
        size_t* sent_length,
        int flags = 0,
        bool auto_restart = true
    );

    /// @return Whether received any data or connect close by peer.
    /// @note If connection is closed by peer, return true and received_size
    ///       is set to 0.
    bool Receive(
        void* buffer,
        size_t buffer_size,
        size_t* received_size,
        int flags = 0,
        bool auto_restart = true
    );

    /// Receive with timeout
    /// @return false if error or timeout, check Socket::GetLastError() for details
    bool Receive(
        void* buffer,
        size_t buffer_size,
        size_t* received_size,
        timeval* timeout,
        int flags = 0,
        bool auto_restart = true
    );
};

/// Stream socket, for example TCP socket
class StreamSocket : public DataSocket
{
public:
    StreamSocket() {}
    explicit StreamSocket(int af, int protocol);

    /// Create a stream socket
    bool Create(sa_family_t af = AF_INET, int protocol = 0);

    /// Shutdown connection
    bool Shutdown();

    /// Shutdown connection sending
    bool ShutdownSend();

    /// Shutdown connection receiving
    bool ShutdownReceive();

    /// @brief Receive data of all expected length
    /// @return Whether received all expacted data
    /// @note If return false, data may also received and received will be
    ///       greater than 0
    bool ReceiveAll(
        void *buffer,
        size_t buffer_size,
        size_t* received_size,
        int flags = 0,
        bool auto_restart = true
    );

    /// @brief Same as upper, expect without the out param 'received_size'
    bool ReceiveAll(
        void *buffer,
        size_t buffer_size,
        int flags = 0,
        bool auto_restart = true
    );

    /// @brief Receive all length, with timeout and out param received_size
    /// @return Whether received all data
    bool ReceiveAll(
        void *buffer,
        size_t buffer_size,
        size_t* received_size,
        timeval* timeout,
        int flags = 0,
        bool auto_restart = true
    );

    /// @brief Receive all length, with timeout
    bool ReceiveAll(
        void *buffer,
        size_t buffer_size,
        timeval* timeout,
        int flags = 0,
        bool auto_restart = true
    );

    /// Receive a line to buffer, include terminal '\n'
    /// @return Whether received a complete line
    bool ReceiveLine(
        void* buffer,
        size_t buffer_size,
        size_t* received_size,
        size_t max_peek_size = 80
    );

    /// Receive a line to string, include terminal '\n'
    /// @return Whether received a complete line
    bool ReceiveLine(std::string* str, size_t peek_size = 80);

    /// Send all data of buffer to socket
    /// @return Whether all data sent
    bool SendAll(
        const void* buffer,
        size_t buffer_size,
        size_t* sent_size,
        int flags = 0,
        bool auto_restart = true
    );

    /// @brief Send all buffer to socket
    /// @return true if all data sent, flase for any other case
    /// @note If false returned, partial data may alse be sent
    bool SendAll(
        const void* buffer,
        size_t buffer_size,
        int flags = 0,
        bool auto_restart = true
    );

    /// @brief Send all buffer to socket with timeout
    /// @return true if all data sent, flase for any other case
    /// @note If false returned, partial data may alse be sent
    bool SendAll(
        const void* buffer,
        size_t buffer_size,
        size_t* sent_size,
        timeval* tv,
        int flags = 0,
        bool auto_restart = true
    );
};

/// Represent a Datagram socket, such as UDP socket
class DatagramSocket : public DataSocket
{
public:
    /// Construct object and create a socket
    DatagramSocket(int af, int protocol = 0);

    /// Construct an empty object
    DatagramSocket() {}

    /// Create the system socket
    bool Create(int af = AF_INET, int protocol = 0);

    /// Send data with specified address
    bool SendTo(
        const void* buffer,
        size_t buffer_size,
        const SocketAddress& address,
        size_t* sent_size
    );

    /// Receive data and obtain remote address
    bool ReceiveFrom(
        void* buffer,
        size_t buffer_size,
        size_t* received_size,
        SocketAddress* address,
        int flags = 0
    );
};

} // namespace toft

// include all inline functions
#include "toft/system/net/socket.inl"

#endif // TOFT_SYSTEM_NET_SOCKET_H
