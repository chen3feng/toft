// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 09/30/11
// Description: IpAddress implementation

#include "toft/system/net/ip_address.h"

namespace toft {

IpAddress::IpAddress(const char* src)
{
    if (!Assign(src))
        throw std::invalid_argument(std::string("Invalid IP Address: ") + src);
}

IpAddress::IpAddress(const std::string& src)
{
    if (!Assign(src))
        throw std::invalid_argument("Invalid IP Address: " + src);
}

bool IpAddress::Assign(const char* src)
{
    int bytes[4];
    char dummy; // catch extra character
    int count = sscanf(src, "%i.%i.%i.%i%c",
                       &bytes[0], &bytes[1], &bytes[2], &bytes[3], &dummy);
    if (count != 4)
        return false;

    for (int i = 0; i < 4; ++i)
    {
        if (bytes[i] < 0 || bytes[i] > UCHAR_MAX)
            return false;
    }

    Assign((unsigned char)bytes[0], (unsigned char)bytes[1],
           (unsigned char)bytes[2], (unsigned char)bytes[3]);
    return true;
}

void IpAddress::ToString(std::string* str) const
{
    char text[INET_ADDRSTRLEN];
    int length = snprintf(text, sizeof(text), "%u.%u.%u.%u",
                          m_bytes[0], m_bytes[1], m_bytes[2], m_bytes[3]);
    str->assign(text, length);
}

#ifdef _WIN32

bool IpAddress::GetLocalList(std::vector<IpAddress>* v)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return false;
    }

    char hostname[128];
    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
        struct hostent* host = gethostbyname(hostname);
        if (host)
        {
            int i = -1;
            do
            {
                i++;
                v->push_back(IpAddress(*reinterpret_cast<IN_ADDR*>
                            (host->h_addr_list[i])));
            } while (host->h_addr_list[i] + host->h_length < host->h_name);
        }
    }
    WSACleanup();
    return true;
}

#else

bool IpAddress::GetLocalList(std::vector<IpAddress>* v)
{
    v->clear();

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        return false;
    }

    // 初始化ifconf
    struct ifconf ifconf;
    char buffer[512];
    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buffer;

    // 获取所有接口信息
    ioctl(sockfd, SIOCGIFCONF, &ifconf);
    close(sockfd);

    struct ifreq *ifreq = reinterpret_cast<struct ifreq*>(buffer);
    for (size_t i = 0; i < (ifconf.ifc_len / sizeof(struct ifreq)); ++i)
    {
        IpAddress ip(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
        if (strcmp(ifreq->ifr_name, "lo") != 0 && !ip.IsLoopback())
        {
            v->push_back(ip);
        }
        ifreq++;
    }
    return true;
}

#endif

bool IpAddress::GetPrivateList(std::vector<IpAddress>* v)
{
    if (!GetLocalList(v))
        return false;

    std::vector<IpAddress>::iterator iter = v->begin();
    while (iter != v->end()) {
        if (!iter->IsPrivate()) {
            iter = v->erase(iter);
        } else {
            ++iter;
        }
    }
    if (v->empty()) {
        return false;
    }
    return true;
}

bool IpAddress::GetPublicList(std::vector<IpAddress>* v)
{
    if (!GetLocalList(v))
        return false;

    std::vector<IpAddress>::iterator iter = v->begin();
    while (iter != v->end()) {
        if (!iter->IsPublic()) {
            iter = v->erase(iter);
        } else {
            ++iter;
        }
    }
    if (v->empty()) {
        return false;
    }
    return true;
}

bool IpAddress::GetFirstLocalAddress(IpAddress* a)
{
    std::vector<IpAddress> v;
    if (GetLocalList(&v)) {
        *a = v[0];
        return true;
    }
    return false;
}

bool IpAddress::GetFirstPrivateAddress(IpAddress* a)
{
    std::vector<IpAddress> v;
    if (GetPrivateList(&v)) {
        *a = v[0];
        return true;
    }
    return false;
}

bool IpAddress::GetFirstPublicAddress(IpAddress* a)
{
    std::vector<IpAddress> v;
    if (GetPublicList(&v)) {
        *a = v[0];
        return true;
    }
    return false;
}

} // namespace toft

