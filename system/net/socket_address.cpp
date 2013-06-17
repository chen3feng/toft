// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 09/30/11
// Description: sockaddr wrapper

#include "toft/system/net/socket_address.h"

// GLOBAL_NOLINT(runtime/printf)

namespace toft {

static bool ParseSocketAddress4(const char* str, int bytes[4], int* port)
{
    char dummy; // catch extra character
    int count = sscanf(str, "%i.%i.%i.%i:%d%c",
                       &bytes[0], &bytes[1], &bytes[2], &bytes[3], port, &dummy);
    if (count != 5)
        return false;
    for (int i = 0; i < 4; ++i)
    {
        if (bytes[i] < 0 || bytes[i] > UCHAR_MAX)
            return false;
    }
    return true;
}

/// Abstract sockaddr

SocketAddressInet4::SocketAddressInet4(const char* src)
{
    if (!Assign(src))
    {
        throw std::runtime_error(std::string("Invalid IPv4 socket address: ") + src);
    }
}

SocketAddressInet4::SocketAddressInet4(const std::string& src)
{
    if (!Assign(src))
    {
        throw std::runtime_error(std::string("Invalid IPv4 socket address: ") + src);
    }
}

SocketAddressInet4::SocketAddressInet4(const char* src, unsigned short port)
{
    if (!Assign(src, port))
    {
        throw std::runtime_error(std::string("Invalid IPv4 socket address: ") + src);
    }
}

SocketAddressInet4::SocketAddressInet4(const std::string& src, unsigned short port)
{
    if (!Assign(src, port))
    {
        throw std::runtime_error(std::string("Invalid IPv4 socket address: ") + src);
    }
}

SocketAddressInet4& SocketAddressInet4::operator=(const SocketAddress& rhs)
{
    if (!CopyFrom(rhs))
        throw std::runtime_error("SocketAddress: Can't copy from " + rhs.ToString());
    return *this;
}

bool SocketAddressInet4::Assign(unsigned int ip, unsigned short port)
{
    m_address.sin_addr.s_addr = ip;
    m_address.sin_port = htons(port);
    return true;
}

bool SocketAddressInet4::Assign(const IpAddress& ip, unsigned short port)
{
    m_address.sin_addr = ip.ToInAddr();
    m_address.sin_port = htons(port);
    return true;
}

bool SocketAddressInet4::Assign(
    unsigned char b1,
    unsigned char b2,
    unsigned char b3,
    unsigned char b4,
    unsigned short port)
{
    unsigned char* p = reinterpret_cast<unsigned char*>(&m_address.sin_addr);
    p[0] = b1;
    p[1] = b2;
    p[2] = b3;
    p[3] = b4;

    m_address.sin_port = htons(port);

    return true;
}

bool SocketAddressInet4::Assign(const char* str)
{
    int bytes[4];
    int port;
    if (!ParseSocketAddress4(str, bytes, &port))
        return false;
    return Assign(
        (unsigned char)bytes[0],
        (unsigned char)bytes[1],
        (unsigned char)bytes[2],
        (unsigned char)bytes[3],
        port
    );
}

bool SocketAddressInet4::Assign(const char* str, unsigned short port)
{
    IpAddress ip;
    if (ip.Assign(str))
        return Assign(ip, port);
    return false;
}

int SocketAddressInet4::Compare(const SocketAddressInet4& rhs) const
{
    if (GetIP() > rhs.GetIP())
        return 1;
    else if (GetIP() < rhs.GetIP())
        return -1;

    return GetPort() - rhs.GetPort();
}

void SocketAddressInet4::DoToString(std::string* str) const
{
    char text[INET_ADDRSTRLEN + sizeof(":65536")];
    const unsigned char* p = reinterpret_cast<const unsigned char*>(&m_address.sin_addr);
    int length = snprintf(
        text, sizeof(text), "%u.%u.%u.%u:%d",
        p[0], p[1], p[2], p[3], ntohs(m_address.sin_port)
    );
    str->assign(text, length);
}

/////////////////////////////////////////////////////////////////////////////
// Socket address encapsulation for IPv6

SocketAddressInet6::SocketAddressInet6(const char* src)
{
    if (!Parse(src))
    {
        throw std::runtime_error(std::string("Invalid IPv6 socket address: ") + src);
    }
}

SocketAddressInet6::SocketAddressInet6(const std::string& src)
{
    if (!Parse(src))
    {
        throw std::runtime_error(std::string("Invalid IPv6 socket address: ") + src);
    }
}

void SocketAddressInet6::DoToString(std::string* str) const
{
    str->clear();
}

bool SocketAddressInet6::DoParse(const char* str)
{
    return false;
}

/////////////////////////////////////////////////////////////////////////////
// SocketAddressInet members

SocketAddressInet::SocketAddressInet(const char* src) : m_address()
{
    if (!Parse(src))
    {
        throw std::runtime_error(std::string("Invalid IPv4/6 socket address: ") + src);
    }
}

SocketAddressInet::SocketAddressInet(const std::string& src) : m_address()
{
    if (!Parse(src))
    {
        throw std::runtime_error(std::string("Invalid IPv4/6 socket address: ") + src);
    }
}

SocketAddressInet::SocketAddressInet(unsigned int ip, unsigned short port) : m_address()
{
    m_address.v4.sin_family = AF_INET;
    m_address.v4.sin_addr.s_addr = ip;
    m_address.v4.sin_port = htons(port);
}

SocketAddressInet::SocketAddressInet(
    unsigned char b1,
    unsigned char b2,
    unsigned char b3,
    unsigned char b4,
    unsigned short port) :
    m_address()
{
    m_address.v4.sin_family = AF_INET;

    unsigned char* p = reinterpret_cast<unsigned char*>(&m_address.v4.sin_addr);
    p[0] = b1;
    p[1] = b2;
    p[2] = b3;
    p[3] = b4;

    m_address.v4.sin_port = htons(port);
}

socklen_t SocketAddressInet::Length() const
{
    switch (m_address.common.sa_family)
    {
    case AF_INET:
        return sizeof(m_address.v4);
#ifdef AF_INET6
    case AF_INET6:
        return sizeof(m_address.v6);
#endif
    default:
        assert(false);
    }
    return false;
}

bool SocketAddressInet::SetLength(socklen_t length)
{
    switch (m_address.common.sa_family)
    {
    case AF_INET:
        return length == sizeof(m_address.v4);
#ifdef AF_INET6
    case AF_INET6:
        return length == sizeof(m_address.v6);
#endif
    default:
        assert(false);
    }
    return false;
}

SocketAddressInet& SocketAddressInet::operator=(const SocketAddress& src)
{
    if (src.Family() != AF_INET && src.Family() != AF_INET6) {
        throw std::runtime_error(
            "SocketAddress: Can't copy from " + src.ToString());
    }
    memcpy(Address(), src.Address(), src.Length());
    return *this;
}

void SocketAddressInet::DoToString(std::string* str) const
{
    char buf[128];
    int length = 0;

    switch (m_address.common.sa_family)
    {
    case AF_INET:
        {
            const unsigned char* p =
                reinterpret_cast<const unsigned char*>(&m_address.v4.sin_addr);
            length = snprintf(
                buf, sizeof(buf),
                "%d.%d.%d.%d:%d", p[0], p[1], p[2], p[3],
                ntohs(m_address.v4.sin_port)
            );
        }
        break;
#ifdef AF_INET6
    case AF_INET6:
        break;
#endif
    default:
        // Return empty string if not a valid IP4/6 socket address
        break;
    }
    str->assign(buf, length);
}

bool SocketAddressInet::DoParse(const char* str)
{
    int bytes[4];
    int port;
    if (!ParseSocketAddress4(str, bytes, &port))
        return false;

    m_address.v4.sin_family = AF_INET;

    unsigned char* p =
        reinterpret_cast<unsigned char*>(&m_address.v4.sin_addr);

    p[0] = (unsigned char) bytes[0];
    p[1] = (unsigned char) bytes[1];
    p[2] = (unsigned char) bytes[2];
    p[3] = (unsigned char) bytes[3];
    m_address.v4.sin_port = htons(port);
    return true;
}

bool SocketAddressInet::DoCopyFrom(const SocketAddress& rhs)
{
    switch (rhs.Family())
    {
    case AF_INET:
    case AF_INET6:
        return true;
    default:
        return false;
    }
}

uint16_t SocketAddressInet::GetPort() const
{
    switch (m_address.common.sa_family) {
    case AF_INET:
        return ntohs(m_address.v4.sin_port);
#ifdef AF_INET6
    case AF_INET6:
        return ntohs(m_address.v6.sin6_port);
#endif
    }
    assert(!"Invalid address family of SocketAddressInet");
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Unix domain socket address
#ifdef __unix__
SocketAddressUnix::~SocketAddressUnix() {}

SocketAddressUnix::SocketAddressUnix(const char* name)
{
    if (!Parse(name))
        throw std::runtime_error(std::string("Invalid unix domain socket address: ") + name);
}

SocketAddressUnix::SocketAddressUnix(const std::string& name)
{
    if (!Parse(name))
        throw std::runtime_error(std::string("Invalid unix domain socket address: ") + name);
}

bool SocketAddressUnix::DoParse(const char* name)
{
    // "/data/local.socket"
    // "@/data/local.socket"
    // check invalid path
    if (name[0] == '/' || (name[0] == '@' && name[1] == '/'))
    {
        size_t length = strlen(name);
        if (length + 1> sizeof(m_address.sun_path))
            return false;
        memcpy(m_address.sun_path, name, length + 1);
        return true;
    }
    return false;
}
#endif

// for store any type socket address
SocketAddressStorage::~SocketAddressStorage() {}

void SocketAddressStorage::DoToString(std::string* str) const
{
    str->clear();
    switch (m_address.ss_family)
    {
    case AF_UNIX:
#ifdef __unix__
        *str = reinterpret_cast<const sockaddr_un&>(m_address).sun_path;
#endif
        break;
    case AF_INET:
        {
            const sockaddr_in* saddrin = reinterpret_cast<const sockaddr_in*>(&m_address);
            char buffer[64];
            const unsigned char* p =
                reinterpret_cast<const unsigned char*>(&saddrin->sin_addr);
            int length = snprintf(
                buffer, sizeof(buffer),
                "%d.%d.%d.%d:%d", p[0], p[1], p[2], p[3],
                ntohs(saddrin->sin_port)
            );
            str->assign(buffer, length);
        }
        break;
#ifdef AF_INET6
    case AF_INET6:
        break;
#endif
    default:
        break;
    };
}

bool SocketAddressStorage::DoParse(const char* str)
{
    // try inet address
    {
        SocketAddressInet address;
        if (address.Parse(str))
        {
            *this = address;
            return true;
        }
    }
#ifdef __unix__
    // try unix domain address
    {
        SocketAddressUnix address;
        if (address.Parse(str))
        {
            *this = address;
            return true;
        }
    }
#endif
    return false;
}

bool SocketAddressStorage::DoCopyFrom(const SocketAddress& rhs)
{
    m_address.ss_family = rhs.Family();
    memcpy(this->Address(), rhs.Address(), rhs.Length());
    return SetLength(rhs.Length());
}

} // namespace toft

