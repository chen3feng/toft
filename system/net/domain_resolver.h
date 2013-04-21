// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 10/27/11
// Description: Internet Domain Resolver

#ifndef TOFT_SYSTEM_NET_DOMAIN_RESOLVER_H
#define TOFT_SYSTEM_NET_DOMAIN_RESOLVER_H
#pragma once

#include <string>
#include <vector>

#include "toft/system/net/ip_address.h"

// global types ford declaration
struct hostent;
struct addrinfo;

namespace toft {

/// This class stores information about a host such as host name, alias names
/// and a list of IP addresses.
class HostEntry
{
public:
    typedef std::vector<std::string> AliasList;
    typedef std::vector<IpAddress>   AddressList;

    /// Creates an empty HostEntry.
    HostEntry();

    /// Creates the HostEntry from the data in a hostent structure.
    void Initialize(const struct hostent* entry);

    /// Creates the HostEntry from the data in a addrinfo structure.
    void Initialize(const struct addrinfo* info);

    /// Destroys the HostEntry.
    ~HostEntry();

    /// Clear all information
    void Clear();

    /// Creates the HostEntry by copying another one.
    HostEntry(const HostEntry& entry);

    /// Assigns another HostEntry.
    HostEntry& operator = (const HostEntry& entry);

    /// Swaps the HostEntry with another one.
    void Swap(HostEntry* host_entry);

    /// Returns the canonical host name.
    const std::string& Name() const;

    /// Returns a vector containing alias names for the host name.
    const AliasList& Aliases() const;

    /// Returns a vector containing the IpAddresses for the host.
    const AddressList& Addresses() const;

private:
    void AddIp(IpAddress ip);

private:
    std::string m_name;
    AliasList   m_aliases;
    AddressList m_addresses;
};

inline const std::string& HostEntry::Name() const
{
    return m_name;
}

inline const HostEntry::AliasList& HostEntry::Aliases() const
{
    return m_aliases;
}

inline const HostEntry::AddressList& HostEntry::Addresses() const
{
    return m_addresses;
}

/// simple internet domain resolver
class DomainResolver
{
public:
    /// perform domain query
    static bool Query(
        const std::string& hostname,
        HostEntry* host_entry,
        int* error_code = NULL
    );

    /// simple query, only ip address returned
    static bool ResolveIpAddress(
        const std::string& hostname,
        std::vector<IpAddress>* ips,
        int* error_code = NULL
    );

    static bool IsFatalError(int error_code);

    /**
     * convert error code to human readable string.
     *
     *  Possible error codes, see netdb.h:
     *
     *  NETDB_SUCCESS
     *     No problem.
     *
     *  HOST_NOT_FOUND
     *     The specified host is unknown.
     *
     *  NO_ADDRESS or NO_DATA
     *     The requested name is valid but does not have an IP address.
     *
     *  NO_RECOVERY
     *     A non-recoverable name server error occurred.
     *
     *  TRY_AGAIN
     *     A temporary error occurred on an authoritative name server.  Try again later.
    */
    static std::string ErrorString(int error_code);
};

} // namespace toft

namespace std {

template <>
inline void swap(toft::HostEntry& h1, toft::HostEntry& h2)
{
    h1.Swap(&h2);
}

} // namespace std

#endif // TOFT_SYSTEM_NET_DOMAIN_RESOLVER_H
