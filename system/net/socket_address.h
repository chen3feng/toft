// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 09/30/11
// Description: sockaddr wrapper

#ifndef TOFT_SYSTEM_NET_SOCKET_ADDRESS_H
#define TOFT_SYSTEM_NET_SOCKET_ADDRESS_H
#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string>

#include "toft/system/net/ip_address.h"

namespace toft {

/// Abstract SocketAddress interface
class SocketAddress
{
protected:
    SocketAddress() {}
    virtual ~SocketAddress() {}

public:
    /// Get address family
    virtual sa_family_t Family() const = 0;

    /// Get as sockaddr*
    sockaddr* Address();

    /// Get as sockaddr*
    const sockaddr* Address() const;

    /// Get address length
    virtual socklen_t Length() const = 0;

    /// Set address length
    virtual bool SetLength(socklen_t length) = 0;

    /// Set max capacity, for variable length address
    virtual socklen_t Capacity() const = 0;
    virtual void Clear() = 0;

    /// Convert to string
    void ToString(std::string* str) const;
    std::string ToString() const;

    bool Parse(const char* str);
    bool Parse(const std::string& str);

    bool CopyFrom(const SocketAddress& rhs);
    SocketAddress& operator=(const SocketAddress& rhs);

private:
    virtual bool DoParse(const char* str) = 0;
    virtual const sockaddr* DoGetAddress() const = 0;
    virtual void DoToString(std::string* str) const = 0;
    virtual bool DoCopyFrom(const SocketAddress& rhs) = 0;
};

inline sockaddr* SocketAddress::Address()
{
    return const_cast<sockaddr*>(DoGetAddress());
}

/// Get as sockaddr*
inline const sockaddr* SocketAddress::Address() const
{
    return DoGetAddress();
}

/// Convert to string
inline void SocketAddress::ToString(std::string* str) const
{
    return DoToString(str);
}

inline std::string SocketAddress::ToString() const
{
    std::string str;
    DoToString(&str);
    return str;
}

inline bool SocketAddress::Parse(const char* str)
{
    return DoParse(str);
}

inline bool SocketAddress::Parse(const std::string& str)
{
    return DoParse(str.c_str());
}

inline bool SocketAddress::CopyFrom(const SocketAddress& rhs)
{
    return DoCopyFrom(rhs);
}

inline SocketAddress& SocketAddress::operator=(const SocketAddress& rhs)
{
    if (!CopyFrom(rhs))
        throw std::runtime_error("SocketAddress: Can't copy from " + rhs.ToString());
    return *this;
}

/// Any sockaddr type encapsulation
template <typename Type, sa_family_t AF>
class SocketAddressTemplate : public SocketAddress
{
protected:
    SocketAddressTemplate() : m_address()
    {
        reinterpret_cast<sockaddr&>(m_address).sa_family = AF;
    }
public:
    virtual sa_family_t Family() const
    {
        return m_generic_address.sa_family;
    }
    virtual socklen_t Length() const
    {
        return sizeof(m_address);
    }
    virtual bool SetLength(socklen_t length)
    {
        return length == sizeof(m_address);
    }
    socklen_t Capacity() const
    {
        return sizeof(m_address);
    }
    virtual void Clear()
    {
        memset(&m_address, 0, sizeof(m_address));
        reinterpret_cast<sockaddr&>(m_address).sa_family = AF;
    }
private:
    virtual const sockaddr* DoGetAddress() const
    {
        return &m_generic_address;
    }
    virtual bool DoCopyFrom(const SocketAddress& rhs)
    {
        if (rhs.Family() != Family())
            return false;
        memcpy(this->Address(), rhs.Address(), rhs.Length());
        return true;
    }
protected:
    union
    {
        Type m_address;
        sockaddr m_generic_address;
#ifdef _WIN32
        unsigned __int64 align;
#endif
    };
};

/// Socket address encapsulation for IPv4
class SocketAddressInet4 : public SocketAddressTemplate<sockaddr_in, AF_INET>
{
    typedef SocketAddressTemplate<sockaddr_in, AF_INET> Base;

public:
    SocketAddressInet4(unsigned int ip, unsigned short port);
    SocketAddressInet4(const IpAddress& ip, unsigned short port);
    SocketAddressInet4() {}
    explicit SocketAddressInet4(const char* src);
    explicit SocketAddressInet4(const std::string& src);
    SocketAddressInet4(const char* src, unsigned short port);
    SocketAddressInet4(const std::string& src, unsigned short port);
    SocketAddressInet4& operator=(const SocketAddress& rhs);
    SocketAddressInet4(
        unsigned char b1,
        unsigned char b2,
        unsigned char b3,
        unsigned char b4,
        unsigned short port
    );

    bool Assign(unsigned int ip, unsigned short port);
    bool Assign(const IpAddress& ip, unsigned short port);
    bool Assign(
        unsigned char b1,
        unsigned char b2,
        unsigned char b3,
        unsigned char b4,
        unsigned short port
    );

    bool Assign(const char* str);
    bool Assign(const char* str, unsigned short port);
    bool Assign(const std::string& ip, unsigned short port);
    bool Assign(const std::string& src);

    const IpAddress GetIP() const;
    void SetIP(const IpAddress& address);

    uint16_t GetPort() const;
    void SetPort(uint16_t port);

    int Compare(const SocketAddressInet4& rhs) const;

private:
    void DoToString(std::string* str) const;
    virtual bool DoParse(const char* src)
    {
        return Assign(src);
    }
};

inline SocketAddressInet4::SocketAddressInet4(
    unsigned int ip,
    unsigned short port)
{
    Assign(ip, port);
}

inline SocketAddressInet4::SocketAddressInet4(
    const IpAddress& ip,
    unsigned short port)
{
    Assign(ip, port);
}

inline SocketAddressInet4::SocketAddressInet4(
    unsigned char b1,
    unsigned char b2,
    unsigned char b3,
    unsigned char b4,
    unsigned short port)
{
    Assign(b1, b2, b3, b4, port);
}

inline bool SocketAddressInet4::Assign(
    const std::string& ip,
    unsigned short port)
{
    return Assign(ip.c_str(), port);
}

inline bool SocketAddressInet4::Assign(const std::string& src)
{
    return Assign(src.c_str());
}

inline void SocketAddressInet4::SetIP(const IpAddress& address)
{
    m_address.sin_addr = address.ToInAddr();
}

inline const IpAddress SocketAddressInet4::GetIP() const
{
    return IpAddress(m_address.sin_addr);
}

inline uint16_t SocketAddressInet4::GetPort() const
{
    return ntohs(m_address.sin_port);
}

inline void SocketAddressInet4::SetPort(uint16_t port)
{
    m_address.sin_port = htons(port);
}

inline bool operator==(const SocketAddressInet4& lhs, const SocketAddressInet4& rhs)
{
    return lhs.Compare(rhs) == 0;
}

inline bool operator!=(const SocketAddressInet4& lhs, const SocketAddressInet4& rhs)
{
    return !(lhs == rhs);
}

inline bool operator<(const SocketAddressInet4& lhs, const SocketAddressInet4& rhs)
{
    return lhs.Compare(rhs) < 0;
}

inline bool operator<=(const SocketAddressInet4& lhs, const SocketAddressInet4& rhs)
{
    return lhs.Compare(rhs) <= 0;
}

inline bool operator>(const SocketAddressInet4& lhs, const SocketAddressInet4& rhs)
{
    return lhs.Compare(rhs) > 0;
}

inline bool operator>=(const SocketAddressInet4& lhs, const SocketAddressInet4& rhs)
{
    return lhs.Compare(rhs) >= 0;
}

#ifdef AF_INET6
/// Socket address encapsulation for IPv6
class SocketAddressInet6 : public SocketAddressTemplate<sockaddr_in6, AF_INET6>
{
public:
    SocketAddressInet6() {}
    explicit SocketAddressInet6(const char* src);
    explicit SocketAddressInet6(const std::string& src);

private:
    // TODO(simonwang): should add support later
    virtual void DoToString(std::string* str) const;
    virtual bool DoParse(const char* str);
};
#endif

class SocketAddressInet : public SocketAddress
{
public:
    SocketAddressInet();
    explicit SocketAddressInet(const char* src);
    explicit SocketAddressInet(const std::string& src);

    SocketAddressInet(unsigned int ip, unsigned short port);
    SocketAddressInet(
        unsigned char b1,
        unsigned char b2,
        unsigned char b3,
        unsigned char b4,
        unsigned short port
    );

    virtual sa_family_t Family() const { return m_address.common.sa_family; }
    virtual socklen_t Length() const;
    virtual bool SetLength(socklen_t length);
    virtual void Clear() { memset(&m_address, 0, sizeof(m_address)); }
    virtual socklen_t Capacity() const { return sizeof(m_address); }
    SocketAddressInet& operator=(const SocketAddress& src);
    uint16_t GetPort() const;

private:
    virtual const sockaddr* DoGetAddress() const;
    virtual void DoToString(std::string* str) const;
    virtual bool DoParse(const char* str);
    virtual bool DoCopyFrom(const SocketAddress& rhs);

private:
    union
    {
        sockaddr common;
        sockaddr_in v4;
#ifdef AF_INET6
        sockaddr_in6 v6;
#endif
    } m_address;
};

inline SocketAddressInet::SocketAddressInet() : m_address()
{
    m_address.common.sa_family = AF_UNSPEC;
}

inline const sockaddr* SocketAddressInet::DoGetAddress() const
{
    return &m_address.common;
}

#ifdef __unix__
/// Unix domain socket address
class SocketAddressUnix : public SocketAddressTemplate<sockaddr_un, AF_UNIX>
{
public:
    SocketAddressUnix() {}
    ~SocketAddressUnix();
    explicit SocketAddressUnix(const char* name);
    explicit SocketAddressUnix(const std::string& name);
    virtual socklen_t Length() const;

private:
    virtual void DoToString(std::string* str) const;
    virtual bool DoParse(const char* name);
};

inline socklen_t SocketAddressUnix::Length() const
{
    return offsetof(struct sockaddr_un, sun_path) + strlen(m_address.sun_path);
}

inline void SocketAddressUnix::DoToString(std::string* str) const
{
    str->assign(m_address.sun_path);
}

typedef SocketAddressUnix SocketAddressLocal;

#endif // __unix__

/// for store any type socket address
class SocketAddressStorage :
    public SocketAddressTemplate<sockaddr_storage, AF_UNSPEC>
{
public:
    SocketAddressStorage() : m_length(0) {}
    virtual ~SocketAddressStorage();
    explicit SocketAddressStorage(const SocketAddress& src);
    SocketAddressStorage& operator=(const SocketAddress& src);
    virtual sa_family_t Family() const { return m_address.ss_family; }
    virtual socklen_t Length() const { return m_length; }
    virtual bool SetLength(socklen_t length);

private:
    virtual const sockaddr* DoGetAddress() const;
    virtual void DoToString(std::string* str) const;
    virtual bool DoParse(const char* str);
    virtual bool DoCopyFrom(const SocketAddress& rhs);

private:
    socklen_t m_length;
};

inline SocketAddressStorage::SocketAddressStorage(const SocketAddress& src)
{
    memcpy(Address(), src.Address(), src.Length());
    m_length = src.Length();
}

inline SocketAddressStorage& SocketAddressStorage::operator=(const SocketAddress& src)
{
    memcpy(Address(), src.Address(), src.Length());
    m_length = src.Length();
    return *this;
}

inline bool SocketAddressStorage::SetLength(socklen_t length)
{
    if (length > (socklen_t)sizeof(m_address))
        return false;
    m_length = length;
    return true;
}

inline const sockaddr* SocketAddressStorage::DoGetAddress() const
{
    return &m_generic_address;
}

} // namespace toft

#endif // TOFT_SYSTEM_NET_SOCKET_ADDRESS_H
