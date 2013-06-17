// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 09/30/11
// Description:

#include "toft/system/net/ip_address.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(IpAddress, Constructor)
{
    EXPECT_NO_THROW(IpAddress a("1.2.3.4"));
    EXPECT_NO_THROW(IpAddress a(std::string("1.2.3.4")));
    EXPECT_ANY_THROW(IpAddress a("1.256.3.4"));
    EXPECT_ANY_THROW(IpAddress a(std::string("1.2.3.4x")));
}

TEST(IpAddress, Parse)
{
    EXPECT_TRUE(IpAddress("1.2.3.4") == IpAddress(1, 2, 3, 4));
    IpAddress address;
    EXPECT_FALSE(address.Assign("-1.2.3.4"));
    EXPECT_FALSE(address.Assign("1.2.3.4x"));
    EXPECT_FALSE(address.Assign("1.256.3.4"));
    EXPECT_TRUE(address.Assign("77.220.81.0017"));
    EXPECT_EQ("77.220.81.15", address.ToString());
}

TEST(IpAddress, Bytes)
{
    IpAddress a("1.2.3.4");
    EXPECT_EQ(1, a.Bytes()[0]);
    EXPECT_EQ(2, a.Bytes()[1]);
    EXPECT_EQ(3, a.Bytes()[2]);
    EXPECT_EQ(4, a.Bytes()[3]);
}

TEST(IpAddress, ByteOrder)
{
    IpAddress a("192.168.0.1");
    EXPECT_EQ(static_cast<in_addr_t>(192 + (168 << 8) + (0 << 16) + (1 << 24)), a.ToInt());
    EXPECT_EQ(static_cast<in_addr_t>((192 << 24)+ (168 << 16) + (0 << 8) + 1), a.ToLocalInt());
}

TEST(IpAddress, CompareToSystemParsing)
{
    IpAddress a("192.168.0.1");
    EXPECT_EQ(inet_addr("192.168.0.1"), a.ToInt());
}

TEST(IpAddress, WellKnown)
{
    EXPECT_TRUE(IpAddress("0.0.0.0") == IpAddress::Any());
    EXPECT_TRUE(IpAddress("127.0.0.1") == IpAddress::Loopback());
    EXPECT_TRUE(IpAddress("255.255.255.255") == IpAddress::Broadcast());
    EXPECT_TRUE(IpAddress("255.255.255.255") == IpAddress::None());
}

TEST(IpAddress, Compare)
{
    EXPECT_TRUE(IpAddress("0.0.0.0") == IpAddress("0.0.0.0"));
    EXPECT_TRUE(IpAddress("0.0.0.0") != IpAddress("0.0.0.1"));
    EXPECT_TRUE(IpAddress("0.0.0.0") < IpAddress("0.0.0.1"));
    EXPECT_TRUE(IpAddress("1.0.0.0") > IpAddress("0.1.1.1"));

    EXPECT_TRUE(IpAddress("0.0.0.1") >= IpAddress("0.0.0.0"));
    EXPECT_TRUE(IpAddress("0.0.0.0") >= IpAddress("0.0.0.0"));
    EXPECT_TRUE(IpAddress("0.0.0.0") >= IpAddress("0.0.0.0"));

    EXPECT_TRUE(IpAddress("0.0.0.0") <= IpAddress("0.0.0.0"));
    EXPECT_TRUE(IpAddress("0.0.0.0") <= IpAddress("0.0.0.1"));
}

TEST(IpAddress, Type)
{
    EXPECT_TRUE(IpAddress("0.0.0.255").IsBroadcast());
    EXPECT_TRUE(!IpAddress("255.0.0.0").IsBroadcast());

    EXPECT_TRUE(IpAddress("127.0.0.1").IsLoopback());
    EXPECT_TRUE(IpAddress("127.1.1.1").IsLoopback());
    EXPECT_TRUE(!IpAddress("128.0.0.1").IsLoopback());

    EXPECT_TRUE(IpAddress("10.0.0.1").IsPrivate());
    EXPECT_TRUE(IpAddress("192.168.0.1").IsPrivate());
    EXPECT_TRUE(IpAddress("172.16.0.1").IsPrivate());
    EXPECT_TRUE(IpAddress("172.31.0.1").IsPrivate());
    EXPECT_TRUE(!IpAddress("172.11.0.1").IsPrivate());
    EXPECT_TRUE(!IpAddress("172.32.0.1").IsPrivate());
}

TEST(IpAddress, GetAddressList)
{
    std::vector<IpAddress> v;
    IpAddress address;
    EXPECT_EQ(IpAddress::GetLocalList(&v), IpAddress::GetFirstLocalAddress(&address));
    EXPECT_EQ(IpAddress::GetPublicList(&v), IpAddress::GetFirstPublicAddress(&address));
    EXPECT_EQ(IpAddress::GetPrivateList(&v), IpAddress::GetFirstPrivateAddress(&address));
}

} // namespace toft
