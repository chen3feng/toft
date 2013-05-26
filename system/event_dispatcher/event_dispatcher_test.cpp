// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/event_dispatcher/event_dispatcher.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(EventDispatcher, Ctor) {
    EventDispatcher dispatcher;
}

TEST(EventDispatcher, Empty) {
    EventDispatcher dispatcher;
    dispatcher.Run(); // Should not block
}

static void OnTimer(bool* received, int) {
    *received = true;
}

TEST(EventDispatcher, Timer) {
    using namespace std::placeholders;
    EventDispatcher dispatcher;
    bool received = false;
    TimerEventWatcher watcher(&dispatcher, std::bind(OnTimer, &received, _1),
                              200);
    watcher.Start();
    dispatcher.Run();
    EXPECT_TRUE(received);
}

static void OnSignal(EventDispatcher* dispatcher, bool* received, int) {
    *received = true;
    dispatcher->Break();
}

TEST(EventDispatcher, Signal) {
    using namespace std::placeholders;
    EventDispatcher dispatcher;
    bool received = false;
    SignalEventWatcher watcher(&dispatcher, std::bind(OnSignal, &dispatcher,
                                                      &received, _1), SIGALRM);
    watcher.Start();
    alarm(1);
    dispatcher.Run();
    EXPECT_TRUE(received);
}

static void OnIo(int fd, char* p, int events) {
    if (read(fd, p, 1) != 1) {
        // Do nothing.
    }
}

TEST(EventDispatcher, Io) {
    using namespace std::placeholders;
    EventDispatcher dispatcher;
    char c = 0;
    int fd = open("/dev/zero", O_RDONLY, 0666);
    ASSERT_NE(-1, fd);
    IoEventWatcher watcher(&dispatcher, std::bind(OnIo, fd, &c, _1),
                           fd, EventMask_Read);
    watcher.Start();
    dispatcher.RunOnce();
    close(fd);
    EXPECT_EQ(0, c);
}

} // namespace toft

