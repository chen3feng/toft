// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 09/30/11
// Description:

#include "toft/base/array_size.h"
#include "toft/system/net/socket_address.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

template <typename T>
static void ParseSocketAddressInetTest()
{
    T address;
    EXPECT_TRUE(address.Parse("0.0.0.0:0"));
    EXPECT_TRUE(address.Parse("1.2.3.4:65535"));
    EXPECT_TRUE(address.Parse("77.220.81.0017:65535"));
    EXPECT_EQ("77.220.81.15:65535", address.ToString());
    EXPECT_FALSE(address.Parse("1020.2.3.4:1"));
    EXPECT_FALSE(address.Parse("1.2345.3.4:1"));
    EXPECT_FALSE(address.Parse("1.2345.3.4:65537"));
    EXPECT_FALSE(address.Parse("hello.25.3.4:65537"));
    EXPECT_FALSE(address.Parse("hello.25.3.4:world"));
}

TEST(SocketAddressInet4, Parse)
{
    ParseSocketAddressInetTest<SocketAddressInet4>();
}

TEST(SocketAddressInet, Parse)
{
    ParseSocketAddressInetTest<SocketAddressInet4>();
}

TEST(SocketAddress, Inet4)
{
    SocketAddressInet4 address1("192.168.0.1:1000");
    EXPECT_EQ("192.168.0.1:1000", address1.ToString());
    SocketAddressInet4 address2("192.168.0.1", 2000);
    EXPECT_EQ("192.168.0.1:2000", address2.ToString());
    address1 = address2;
    EXPECT_EQ("192.168.0.1:2000", address1.ToString());
    EXPECT_EQ(address1.ToString(), address2.ToString());
    unsigned int ip = inet_addr("192.168.1.1");
    address1.Assign(ip, 3000);
    EXPECT_EQ("192.168.1.1:3000", address1.ToString());
    address2.Assign("192.168.2.1", 3000);
    EXPECT_EQ("192.168.2.1:3000", address2.ToString());
    EXPECT_LT(0, address2.Compare(address1));
}

TEST(SocketAddress, Inet4Compare)
{
    SocketAddressInet4 address1("192.168.1.1:1000");
    SocketAddressInet4 address2("192.168.2.1:1000");
    SocketAddressInet4 address3("192.168.2.1:2000");
    SocketAddressInet4 address4("192.168.2.1:2000");
    EXPECT_LT(0, address2.Compare(address1));
    EXPECT_GT(0, address1.Compare(address2));
    EXPECT_GT(0, address2.Compare(address3));
    EXPECT_EQ(0, address3.Compare(address4));
}

TEST(SocketAddress, Inet4BadAddress)
{
    const char* bad_address[] = {
        "192.168.0.1.1",
        "300.168.0.1",
        "192.168.1.1.1",
    };
    for (size_t i = 0; i < TOFT_ARRAY_SIZE(bad_address); ++i) {
        EXPECT_ANY_THROW(SocketAddressInet4 a(bad_address[i]));
        EXPECT_ANY_THROW(SocketAddressInet4 a(bad_address[i], 1000));
        EXPECT_ANY_THROW(SocketAddressInet4 a((std::string(bad_address[i]))));
        EXPECT_ANY_THROW(SocketAddressInet4 a(std::string(bad_address[i]), 1000));
    }
}

TEST(SocketAddress, Inet)
{
    SocketAddressInet address1;
    EXPECT_EQ(AF_UNSPEC, address1.Family());
    SocketAddressInet address2("192.168.0.1:1000");
    EXPECT_EQ(AF_INET, address2.Family());
    EXPECT_EQ("192.168.0.1:1000", address2.ToString());
    SocketAddressInet address3("192.168.1.1:2000");
    EXPECT_EQ("192.168.1.1:2000", address3.ToString());
    address2 = address3;
    EXPECT_EQ(address2.ToString(), address3.ToString());
    address2.Clear();
    EXPECT_EQ("", address2.ToString());

    unsigned int ip = inet_addr("192.168.0.1");
    SocketAddressInet address4(ip, 1000);
    EXPECT_EQ("192.168.0.1:1000", address4.ToString());

    unsigned char* b = reinterpret_cast<unsigned char*>(&ip);
    SocketAddressInet address5(b[0], b[1], b[2], b[3], 1000);
    EXPECT_EQ("192.168.0.1:1000", address5.ToString());
}

TEST(SocketAddress, InetGetPort)
{
    SocketAddressInet address1("192.168.1.1:1000");
    EXPECT_EQ(1000, address1.GetPort());

    SocketAddressInet4 address2("192.168.1.1:2000");
    EXPECT_EQ(2000, address2.GetPort());
}

TEST(SocketAddress, CopyFrom)
{
    SocketAddressInet address1("192.168.1.1:1000");
    SocketAddressInet address2("192.168.2.1:2000");
    SocketAddressInet4 address3("192.168.1.2:2000");
    SocketAddressInet4 address4("192.168.2.2:3000");
    SocketAddressUnix address5("/data/local.socket");
    address1 = address2;
    EXPECT_EQ(address1.ToString(), address2.ToString());
    address1 = address3;
    EXPECT_EQ(address1.ToString(), address3.ToString());
    EXPECT_ANY_THROW({ address1 = address5; });  // NOLINT(whitespace/newline)
    address3 = address4;
    EXPECT_EQ(address3.ToString(), address4.ToString());
    address3 = address2;
    EXPECT_EQ(address3.ToString(), address2.ToString());
    EXPECT_ANY_THROW({ address3 = address5; });  // NOLINT(whitespace/newline)
}

TEST(SocketAddress, InetBadAddress)
{
    static const char* bad_address[] = {
        "192.168.0.1.1",
        "300.168.0.1",
        "192.168.1.1.1",
    };
    for (size_t i = 0; i < TOFT_ARRAY_SIZE(bad_address); ++i) {
        EXPECT_ANY_THROW(SocketAddressInet a(bad_address[i]));
        EXPECT_ANY_THROW(SocketAddressInet a((std::string(bad_address[i]))));
    }
}

#ifdef AF_INET6
TEST(SocketAddress, Inet6)
{
    // TODO(simonwang)
    // SocketAddressInet6 parse not implemented yet.
    SocketAddressInet6 address;
    EXPECT_FALSE(address.Parse("2000:0:0:0:0:0:0:1:2"));
    EXPECT_EQ("", address.ToString());
}

TEST(SocketAddress, Inet6BadAddress)
{
    // TODO(simonwang)
    // SocketAddressInet6 parse not implemented yet,
    static const char* bad_address[] = {
        "192.168.1.1",
        "CDMN:910A:2222:5498:8475:1111:3900:2020",
        "2000:0:0:0:0:0:0:1:2",
    };
    for (size_t i = 0; i < TOFT_ARRAY_SIZE(bad_address); ++i) {
        EXPECT_ANY_THROW(SocketAddressInet6 a(bad_address[i]));
        EXPECT_ANY_THROW(SocketAddressInet6 a((std::string(bad_address[i]))));
    }
}
#endif

#ifdef __unix__
TEST(SocketAddress, Unix)
{
    SocketAddressUnix address1;
    EXPECT_EQ("", address1.ToString());
    SocketAddressUnix address2("/data/local.socket");
    EXPECT_EQ("/data/local.socket", address2.ToString());
    SocketAddressUnix address3("@/data/local.socket");
    EXPECT_EQ("@/data/local.socket", address3.ToString());
    address2.Clear();
    EXPECT_EQ("", address2.ToString());
}

TEST(SocketAddress, UnixBadAddress)
{
    const char* bad_address[] = {
        "abc/data/local/socket",
    };
    for (size_t i = 0; i < TOFT_ARRAY_SIZE(bad_address); ++i) {
        EXPECT_ANY_THROW(SocketAddressUnix a(bad_address[i]));
        EXPECT_ANY_THROW(SocketAddressUnix(std::string(bad_address[i])));
    }
}
#endif

TEST(SocketAddress, Storage)
{
    SocketAddressInet address("192.168.0.1:1000");
    SocketAddressStorage storage;
    storage = address;
    EXPECT_EQ(address.ToString(), storage.ToString());
    SocketAddressStorage storage2;
    storage2 = storage;
    EXPECT_EQ(storage.ToString(), storage2.ToString());
    EXPECT_EQ(AF_INET, storage.Family());
    EXPECT_EQ(16U, storage.Length());

    EXPECT_TRUE(storage.Parse("192.168.0.1:1000"));
    EXPECT_EQ("192.168.0.1:1000", storage.ToString());
}

TEST(SocketAddress, Assign)
{
    SocketAddressInet address("192.168.0.1:1000");
    SocketAddressStorage storage;
    SocketAddress& ss = storage;
    ss = address;
    EXPECT_EQ(address.ToString(), ss.ToString());
}

TEST(SocketAddress, Empty)
{
    SocketAddressInet inet;
    EXPECT_EQ("", inet.ToString());

    SocketAddressStorage storage;
    EXPECT_EQ("", storage.ToString());
}

} // namespace toft
