// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/net/socket.h"
#include "toft/system/threading/thread.h"
#
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(Socket, ListenerSocket)
{
    ListenerSocket listener;
    EXPECT_TRUE(listener.Create(AF_INET, SOCK_STREAM));
    listener.SetReuseAddress();
    SocketAddressInet address("127.0.0.1:0");
    SocketAddressInet bind_address;
    EXPECT_TRUE(listener.Bind(address));
    EXPECT_TRUE(listener.GetLocalAddress(&bind_address));
    EXPECT_TRUE(listener.Listen());

    EXPECT_ANY_THROW(ListenerSocket listener2(bind_address, SOCK_STREAM));
}

TEST(Socket, SocketOption)
{
    StreamSocket socket;
    EXPECT_TRUE(socket.Create(AF_INET));
    EXPECT_TRUE(socket.SetCloexec(true));
    EXPECT_TRUE(socket.SetLinger(true, 5));
    struct linger l;
    EXPECT_TRUE(socket.GetOption(SOL_SOCKET, SO_LINGER, &l));
    EXPECT_TRUE(l.l_onoff);
    EXPECT_EQ(5, l.l_linger);
}

TEST(Socket, Attach)
{
    StreamSocket socket;
    EXPECT_TRUE(socket.Create(AF_INET));
    int handle, old_handle;
    handle = socket.Handle();
    EXPECT_LT(-1, handle);
    old_handle = socket.Detach();
    EXPECT_EQ(handle, old_handle);
    EXPECT_EQ(-1, socket.Handle());
}

class DatagramSocketTest : public testing::Test
{
protected:
    DatagramSocketTest()
    {
        EXPECT_TRUE(m_listener.Create(AF_INET));
        EXPECT_TRUE(m_listener.SetReuseAddress());
        SocketAddressInet address("127.0.0.1:0");
        EXPECT_TRUE(m_listener.Bind(address));
        EXPECT_TRUE(m_listener.GetLocalAddress(&m_address));
        m_thread.Start(std::bind(&DatagramSocketTest::IoThread, this));
    }

    ~DatagramSocketTest()
    {
        m_thread.Join();
    }

private:
    void IoThread()
    {
        char buffer[1024];
        size_t buffer_size = 1024;
        SocketAddressInet address;
        size_t received_size;
        size_t sent_size;
        while (m_listener.ReceiveFrom(buffer, buffer_size, &received_size,
                                      &address)) {
            if (!strncasecmp(buffer, "quit", 4)) {
                break;
            }
            m_listener.SendTo(buffer, received_size, address, &sent_size);
            EXPECT_EQ(received_size, sent_size);
        }
    }

protected:
    DatagramSocket m_listener;
    SocketAddressInet m_address;
    Thread m_thread;
};

TEST_F(DatagramSocketTest, SendReceive)
{
    DatagramSocket socket;
    EXPECT_TRUE(socket.Create(AF_INET));
    std::string sent = "12345\n";
    std::string quit = "quit";
    size_t sent_size, received_size;
    char buffer[6];
    size_t buffer_size = 6;
    EXPECT_TRUE(socket.SendTo(sent.data(), sent.size(),
                              m_address, &sent_size));
    EXPECT_EQ(sent.size(), sent_size);
    EXPECT_TRUE(socket.ReceiveFrom(buffer, buffer_size,
                                   &received_size, &m_address));
    EXPECT_EQ(sent.size(), received_size);
    EXPECT_STREQ(sent.data(), buffer);
    EXPECT_TRUE(socket.SendTo(quit.data(), quit.size(),
                              m_address, &sent_size));
}

class StreamSocketTest : public testing::Test
{
protected:
    StreamSocketTest()
    {
        EXPECT_TRUE(m_listener.Create(AF_INET, SOCK_STREAM));
        EXPECT_TRUE(m_listener.SetReuseAddress());
        SocketAddressInet address("127.0.0.1:0");
        EXPECT_TRUE(m_listener.Bind(address));
        EXPECT_TRUE(m_listener.GetLocalAddress(&m_address));
        EXPECT_TRUE(m_listener.Listen());
        m_thread.Start(std::bind(&StreamSocketTest::IoThread, this));
    }
    ~StreamSocketTest()
    {
        m_thread.Join();
    }

private:
    void IoThread()
    {
        StreamSocket acceptor;
        EXPECT_TRUE(m_listener.Accept(&acceptor));
        SocketAddressInet peer_address;
        EXPECT_TRUE(acceptor.GetPeerAddress(&peer_address));
#if __unix__
        int keep_alive;
        int idle;
        int interval;
        int count;
        EXPECT_TRUE(acceptor.SetTcpKeepAliveOption(14400, 150, 5));
        EXPECT_TRUE(acceptor.GetOption(SOL_SOCKET, SO_KEEPALIVE, &keep_alive));
        EXPECT_EQ(1, keep_alive);
        EXPECT_TRUE(acceptor.GetOption(SOL_TCP, TCP_KEEPIDLE, &idle));
        EXPECT_EQ(14400, idle);
        EXPECT_TRUE(acceptor.GetOption(SOL_TCP, TCP_KEEPINTVL, &interval));
        EXPECT_EQ(150, interval);
        EXPECT_TRUE(acceptor.GetOption(SOL_TCP, TCP_KEEPCNT, &count));
        EXPECT_EQ(5, count);
#endif
        std::string line;
        while (acceptor.ReceiveLine(&line) && !line.empty())
        {
            acceptor.SendAll(line.data(), line.size());
        }
    }

protected:
    ListenerSocket m_listener;
    SocketAddressInet m_address;
    Thread m_thread;
};

TEST_F(StreamSocketTest, ConnectWithTimeout)
{
    StreamSocket stream;
    EXPECT_TRUE(stream.Create(AF_INET));
    EXPECT_FALSE(stream.Connect(SocketAddressInet("0.0.0.0:0"), 10));

    bool blocking = true;

    // For unknown reason, if we reuse failed socket, the following Connect
    // will faile with ECONNRESET, create a new socket here.
    EXPECT_TRUE(stream.Create(AF_INET));
    EXPECT_TRUE(stream.GetBlocking(&blocking));
    EXPECT_TRUE(blocking);

    EXPECT_TRUE(stream.Connect(m_address, 10));

    EXPECT_TRUE(stream.GetBlocking(&blocking));
    EXPECT_TRUE(blocking);

    EXPECT_TRUE(stream.SendAll("\n", 1));
}

TEST_F(StreamSocketTest, StreamSocketReceiveLineInString)
{
    StreamSocket stream;
    EXPECT_TRUE(stream.Create(AF_INET));
    EXPECT_FALSE(stream.Connect(SocketAddressInet("0.0.0.0:0")));
    EXPECT_TRUE(stream.Connect(m_address)) << m_address.ToString();
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    EXPECT_TRUE(stream.WaitWriteable(&tv, true));
    std::string sent = "12345\n";
    EXPECT_TRUE(stream.SendAll(sent.data(), sent.size()));
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    EXPECT_TRUE(stream.WaitReadable(&tv, true));
    std::string received;
    EXPECT_TRUE(stream.ReceiveLine(&received));
    EXPECT_EQ(sent, received);

    // Send all with timeout
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    size_t sent_size;
    EXPECT_TRUE(stream.SendAll(sent.data(), sent.size(), &sent_size, &tv));
    EXPECT_EQ(sent.size(), sent_size);
    // peek size with 2
    EXPECT_TRUE(stream.ReceiveLine(&received, 2));
    EXPECT_EQ(sent, received);
}

TEST_F(StreamSocketTest, StreamSocketReceiveLineInSmallerSizeBuffer)
{
    StreamSocket stream;
    EXPECT_TRUE(stream.Create(AF_INET));
    EXPECT_FALSE(stream.Connect(SocketAddressInet("0.0.0.0:0")));
    EXPECT_TRUE(stream.Connect(m_address)) << m_address.ToString();
    std::string sent = "12345\n";
    char buffer[5];
    size_t buffer_size = 5;
    size_t received_size;
    EXPECT_TRUE(stream.SendAll(sent.data(), sent.size()));
    EXPECT_FALSE(stream.ReceiveLine(buffer, buffer_size, &received_size));
    EXPECT_EQ(5U, received_size);
    for (size_t i = 0; i < received_size; ++i) {
        EXPECT_EQ(sent[i], buffer[i]);
    }
    // receive until '\n'
    EXPECT_TRUE(stream.ReceiveLine(buffer, buffer_size, &received_size));
    EXPECT_EQ(1U, received_size);
    EXPECT_EQ('\n', buffer[0]);
}

TEST_F(StreamSocketTest, StreamSocketReceiveLineInEqualSizeBuffer)
{
    StreamSocket stream;
    EXPECT_TRUE(stream.Create(AF_INET));
    EXPECT_TRUE(stream.Connect(m_address)) << m_address.ToString();
    std::string sent = "12345\n";
    char buffer[6];
    size_t received_size;
    size_t buffer_size = 6;
    EXPECT_TRUE(stream.SendAll(sent.data(), sent.size()));
    EXPECT_TRUE(stream.ReceiveLine(buffer, buffer_size, &received_size));
    EXPECT_EQ(6U, received_size);
    EXPECT_STREQ(sent.data(), buffer);
}

TEST_F(StreamSocketTest, StreamSocketReceiveLineInLargerSizeBuffer)
{
    StreamSocket stream;
    EXPECT_TRUE(stream.Create(AF_INET));
    EXPECT_TRUE(stream.Connect(m_address)) << m_address.ToString();
    std::string sent = "12345\n";
    char buffer[10];
    size_t buffer_size = 10;
    size_t received_size;
    EXPECT_TRUE(stream.SendAll(sent.data(), sent.size()));
    EXPECT_TRUE(stream.ReceiveLine(buffer, buffer_size, &received_size));
    EXPECT_EQ(6U, received_size);
    for (size_t i = 0; i < received_size; ++i) {
        EXPECT_EQ(sent[i], buffer[i]);
    }
}

TEST_F(StreamSocketTest, StreamSocketReceiveAll)
{
    StreamSocket stream;
    EXPECT_TRUE(stream.Create(AF_INET));
    EXPECT_TRUE(stream.Connect(m_address)) << m_address.ToString();
    std::string sent = "12345\n";
    EXPECT_TRUE(stream.SendAll(sent.data(), sent.size()));
    char buffer1[6], buffer2[5];
    size_t buffer1_size = 6;
    size_t buffer2_size = 5;
    size_t received_size;
    EXPECT_TRUE(stream.ReceiveAll(buffer1, buffer1_size));
    EXPECT_STREQ(sent.data(), buffer1);

    // with timeout
    EXPECT_TRUE(stream.SendAll(sent.data(), sent.size()));
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    EXPECT_TRUE(stream.ReceiveAll(buffer1, buffer1_size, &received_size, &tv));
    EXPECT_EQ(6U, received_size);
    EXPECT_STREQ(sent.data(), buffer1);

    EXPECT_TRUE(stream.SendAll(sent.data(), sent.size()));
    EXPECT_TRUE(stream.ReceiveAll(buffer2, buffer2_size, &received_size));
    EXPECT_EQ(5U, received_size);
    for (size_t i = 0; i < received_size; ++i) {
        EXPECT_EQ(sent[i], buffer2[i]);
    }
}

} // namespace toft
