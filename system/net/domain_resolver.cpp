// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 10/27/11
// Description:

#include "toft/system/net/domain_resolver.h"

#ifndef _WIN32
#include <netdb.h>
#endif
#include <algorithm>

#include "toft/system/net/os_socket.h"

#ifdef _WIN32
#ifndef NETDB_SUCCESS
#define NETDB_SUCCESS   0   /** no problem */
#endif
#endif

namespace toft {

static void SetErrorCode(int* error_code, int value)
{
    if (error_code)
        *error_code = value;
}

/////////////////////////////////////////////////////////////////////////////
// HostEntry class implementation

HostEntry::HostEntry()
{
}

HostEntry::~HostEntry()
{
}

void HostEntry::AddIp(IpAddress ip)
{
    // make sure ip address are unique and keep same order as query result
    if (std::find(m_addresses.begin(), m_addresses.end(), ip) == m_addresses.end())
        m_addresses.push_back(ip);
}

void HostEntry::Initialize(const struct hostent* entry)
{
    m_name = entry->h_name;

    char** alias = entry->h_aliases;
    if (alias)
    {
        while (*alias)
        {
            m_aliases.push_back(std::string(*alias));
            ++alias;
        }
    }

    char** address = entry->h_addr_list;
    if (address && entry->h_addrtype == AF_INET)
    {
        while (*address)
        {
            AddIp(IpAddress(*reinterpret_cast<in_addr*>(*address)));
            ++address;
        }
    }
}

void HostEntry::Initialize(const struct addrinfo* ainfo)
{
    for (const struct addrinfo* ai = ainfo; ai; ai = ai->ai_next)
    {
        if (ai->ai_canonname)
            m_name.assign(ai->ai_canonname);
        else if (ai->ai_addrlen && ai->ai_addr && ai->ai_family == AF_INET)
            AddIp(IpAddress(reinterpret_cast<sockaddr_in*>(ai->ai_addr)->sin_addr));
    }
}

HostEntry::HostEntry(const HostEntry& entry):
    m_name(entry.m_name),
    m_aliases(entry.m_aliases),
    m_addresses(entry.m_addresses)
{
}

void HostEntry::Clear()
{
    m_name.clear();
    m_aliases.clear();
    m_addresses.clear();
}

HostEntry& HostEntry::operator = (const HostEntry& entry)
{
    if (&entry != this)
    {
        m_name = entry.m_name;
        m_aliases = entry.m_aliases;
        m_addresses = entry.m_addresses;
    }
    return *this;
}

void HostEntry::Swap(HostEntry* other)
{
    std::swap(m_name, other->m_name);
    std::swap(m_aliases, other->m_aliases);
    std::swap(m_addresses, other->m_addresses);
}

/////////////////////////////////////////////////////////////////////////////
// DomainResolver class implementation

bool DomainResolver::Query(
    const std::string& hostname,
    HostEntry* host_entry,
    int* error_code)
{
    struct hostent* he = NULL;
    int error = 0;
#ifdef _GNU_SOURCE
    char buf[4096];
    struct hostent he_buf;
    gethostbyname_r(hostname.c_str(), &he_buf, buf, sizeof(buf), &he, &error);
#else
    he = gethostbyname(hostname.c_str());
    if (!he)
        error = h_errno;
#endif
    if (!he)
    {
        SetErrorCode(error_code, error);
        return false;
    }
    host_entry->Initialize(he);

    SetErrorCode(error_code, 0);
    return true;
}

bool DomainResolver::ResolveIpAddress(
    const std::string& hostname,
    std::vector<IpAddress>* ips,
    int* error_code)
{
    HostEntry host_entry;
    if (Query(hostname, &host_entry, error_code))
    {
        *ips = host_entry.Addresses();
        return true;
    }
    return false;
}

bool DomainResolver::IsFatalError(int error_code)
{
    return error_code != NETDB_SUCCESS && error_code != TRY_AGAIN;
}

std::string DomainResolver::ErrorString(int error_code)
{
#ifndef _WIN32
    return hstrerror(error_code);
#else
    return strerror(error_code);
#endif
}

} // namespace toft

