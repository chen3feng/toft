// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 09/30/11
// Description: IP address encapsulation

#ifndef TOFT_SYSTEM_NET_IP_ADDRESS_H
#define TOFT_SYSTEM_NET_IP_ADDRESS_H
#pragma once

#include <limits.h>
#include <stdio.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "toft/system/net/os_socket.h"

namespace toft {

/// IP v4 address
class IpAddress
{
    typedef unsigned char BytesType[4];
public:
    IpAddress() : m_ip() {}
    explicit IpAddress(const char* src);
    explicit IpAddress(const std::string& src);
    explicit IpAddress(unsigned int ip);
    explicit IpAddress(in_addr addr);
    IpAddress(
        unsigned char b1,
        unsigned char b2,
        unsigned char b3,
        unsigned char b4);

public:
    void Assign(unsigned int ip); // network byte order
    void Assign(in_addr addr);
    bool Assign(const char* src);
    bool Assign(const std::string& src);
    void Assign(
        unsigned char b1,
        unsigned char b2,
        unsigned char b3,
        unsigned char b4);

    void Clear();

    in_addr_t ToInt() const;
    in_addr_t ToLocalInt() const;
    const in_addr ToInAddr() const;

    void ToString(std::string* str) const;
    const std::string ToString() const;

    bool IsLoopback() const;
    bool IsBroadcast() const;
    bool IsPrivate() const;
    bool IsLinkLocal() const;
    bool IsPublic() const;

    const BytesType& Bytes() const;

public:
    static const IpAddress None();
    static const IpAddress Any();
    static const IpAddress Broadcast();
    static const IpAddress Loopback();

public:
    /// get local ip address list (except loopback)
    static bool GetLocalList(std::vector<IpAddress>* v);

    /// get local private ip address list
    static bool GetPrivateList(std::vector<IpAddress>* v);

    /// get local public ip address list
    static bool GetPublicList(std::vector<IpAddress>* v);

    /// get first local address
    static bool GetFirstLocalAddress(IpAddress* a);

    /// get first private address
    static bool GetFirstPrivateAddress(IpAddress* a);

    /// get first public address
    static bool GetFirstPublicAddress(IpAddress* a);

private:
    union
    {
        in_addr m_ip;
        unsigned char m_bytes[4];
    };
};

inline IpAddress::IpAddress(unsigned int ip)
{
    Assign(ip);
}

inline IpAddress::IpAddress(
    unsigned char b1,
    unsigned char b2,
    unsigned char b3,
    unsigned char b4)
{
    Assign(b1, b2, b3, b4);
}

inline IpAddress::IpAddress(in_addr addr)
{
    Assign(addr);
}

inline void IpAddress::Assign(unsigned int ip) // network byte order
{
    m_ip.s_addr = ip;
}

inline void IpAddress::Assign(in_addr addr)
{
    m_ip = addr;
}

inline void IpAddress::Assign(
    unsigned char b1,
    unsigned char b2,
    unsigned char b3,
    unsigned char b4)
{
    m_bytes[0] = b1;
    m_bytes[1] = b2;
    m_bytes[2] = b3;
    m_bytes[3] = b4;
}

inline bool IpAddress::Assign(const std::string& src)
{
    return Assign(src.c_str());
}

inline void IpAddress::Clear()
{
    m_ip.s_addr = 0;
}

inline in_addr_t IpAddress::ToInt() const
{
    return m_ip.s_addr;
}

inline in_addr_t IpAddress::ToLocalInt() const
{
    return ntohl(m_ip.s_addr);
}

inline const in_addr IpAddress::ToInAddr() const
{
    return m_ip;
}

inline const std::string IpAddress::ToString() const
{
    std::string result;
    ToString(&result);
    return result;
}

inline bool IpAddress::IsLoopback() const
{
    return m_bytes[0] == 127;
}

inline bool IpAddress::IsBroadcast() const
{
    return m_bytes[3] == 255;
}

inline bool IpAddress::IsPrivate() const
{
    return m_bytes[0] == 10 ||
        (m_bytes[0] == 172 && m_bytes[1] >= 16 && m_bytes[1] <= 31) ||
        (m_bytes[0] == 192 && m_bytes[1] == 168);
}

inline bool IpAddress::IsLinkLocal() const
{
    return m_bytes[0] == 169 && m_bytes[1] == 254;
}

inline bool IpAddress::IsPublic() const
{
    return !IsPrivate() && !IsLinkLocal() && !IsLoopback();
}

inline const IpAddress::BytesType& IpAddress::Bytes() const
{
    return m_bytes;
}

inline const IpAddress IpAddress::None()
{
    return IpAddress(htonl(INADDR_NONE));
}

inline const IpAddress IpAddress::Any()
{
    return IpAddress(htonl(INADDR_ANY));
}

inline const IpAddress IpAddress::Broadcast()
{
    return IpAddress(htonl(INADDR_BROADCAST));
}

inline const IpAddress IpAddress::Loopback()
{
    return IpAddress(htonl(INADDR_LOOPBACK));
}

inline bool operator==(const IpAddress& lhs, const IpAddress& rhs)
{
    return lhs.ToInt() == rhs.ToInt();
}

inline bool operator!=(const IpAddress& lhs, const IpAddress& rhs)
{
    return lhs.ToInt() != rhs.ToInt();
}

inline bool operator<(const IpAddress& lhs, const IpAddress& rhs)
{
    return lhs.ToLocalInt() < rhs.ToLocalInt();
}

inline bool operator>(const IpAddress& lhs, const IpAddress& rhs)
{
    return lhs.ToLocalInt() > rhs.ToLocalInt();
}

inline bool operator<=(const IpAddress& lhs, const IpAddress& rhs)
{
    return lhs.ToLocalInt() <= rhs.ToLocalInt();
}

inline bool operator>=(const IpAddress& lhs, const IpAddress& rhs)
{
    return lhs.ToLocalInt() >= rhs.ToLocalInt();
}

} // namespace toft

#endif // TOFT_SYSTEM_NET_IP_ADDRESS_H
