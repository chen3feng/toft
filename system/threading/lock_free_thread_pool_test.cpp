// Copyright (C) 2013, The Toft Authors.
// Author: An Qin <anqin.qin@gmail.com>
//
// Description:

#include "toft/system/threading/lock_free_thread_pool.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "toft/base/closure.h"
#include "toft/base/functional.h"

namespace toft {

class Foo {
public:
    void test1() {}

    void test2(int32_t param1) {}
};

TEST(LockFreeThreadPool, Closure) {
    LockFreeThreadPool threadpool(4, 4);
    Foo foo;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 20; ++j) {
            threadpool.AddTask(NewClosure(&foo, &Foo::test1));
            threadpool.AddTask(
                NewClosure(
                    &foo, &Foo::test2, static_cast<intptr_t>(i*20+j)));
        }
    }
}

TEST(LockFreeThreadPool, Function) {
    LockFreeThreadPool threadpool(4, 4);
    Foo foo;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 20; ++j) {
            threadpool.AddTask(NewClosure(&foo, &Foo::test1));
            threadpool.AddTask(
                std::bind(&Foo::test2, &foo, static_cast<intptr_t>(i*20+j)));
        }
    }
}

TEST(LockFreeThreadPool, Performance) {
    LockFreeThreadPool threadpool(1000, 1000);
    Foo foo;
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 200; ++j) {
            threadpool.AddTask(NewClosure(&foo, &Foo::test1));
            threadpool.AddTask(
                NewClosure(
                    &foo, &Foo::test2, static_cast<intptr_t>(i*20+j)));
        }
    }
}

} // namespace toft
