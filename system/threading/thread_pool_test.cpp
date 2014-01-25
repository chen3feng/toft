// Copyright (C) 2013, The Toft Authors.
// Author: An Qin <anqin.qin@gmail.com>
//
// Description:

#include "toft/system/threading/thread_pool.h"

#include "toft/base/closure.h"
#include "toft/base/functional.h"
#include "toft/system/threading/this_thread.h"
#include "toft/system/time/clock.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

class Foo {
public:
    void test1() {}

    void test2(int32_t param1) {}
};

TEST(ThreadPool, Closure) {
    ThreadPool threadpool(4, 4);
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

TEST(ThreadPool, Function) {
    ThreadPool threadpool(4, 4);
    Foo foo;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 20; ++j) {
            threadpool.AddTask(NewClosure(&foo, &Foo::test1));
            threadpool.AddTask(
                std::bind(&Foo::test2, &foo, static_cast<intptr_t>(i*20+j)));
        }
    }
}

static void DoNothong()
{
}

TEST(ThreadPool, MessTasks)
{
    ThreadPool threadpool(0, 4);
    for (int i = 0; i < 10000; ++i)
    {
        threadpool.AddTask(DoNothong);
    }
}

TEST(ThreadPool, Terminate)
{
    ThreadPool threadpool;
    for (int i = 0; i < 10000; ++i)
        threadpool.AddTask(DoNothong);
    threadpool.Terminate();
    threadpool.Terminate();
}

TEST(ThreadPool, AfterBusy)
{
    ThreadPool threadpool(0, 4);
    for (int i = 0; i < 1000; ++i)
    {
        threadpool.AddTask(DoNothong);
    }
    ThisThread::Sleep(10);
    for (int i = 0; i < 1000; ++i)
    {
        threadpool.AddTask(DoNothong);
    }
}

static void test_blocking()
{
    ThisThread::Sleep(10);
}

TEST(ThreadPool, MixedBlockingAndNonblocking)
{
    ThreadPool threadpool(0, 4);
    threadpool.AddTask(test_blocking);
    threadpool.AddTask(test_blocking);
    threadpool.AddTask(test_blocking);
    for (int i = 0; i < 10; ++i)
    {
        threadpool.AddTask(DoNothong);
    }
}

TEST(ThreadPool, Blocking)
{
    ThreadPool threadpool(0, 4);
    threadpool.AddTask(test_blocking);
    threadpool.AddTask(test_blocking);
    threadpool.AddTask(test_blocking);
    threadpool.AddTask(test_blocking);
}

static void test_sleep()
{
    ThisThread::Sleep(1);
}

TEST(ThreadPool, SlowCall)
{
    ThreadPool threadpool;
    for (int i = 0; i < 10; ++i)
    {
        threadpool.AddTask(test_sleep);
    }
}

const int kLoopCount = 500000;

class ThreadPoolTest : public testing::TestWithParam<int> {};

TEST_P(ThreadPoolTest, Performance)
{
    int num_threads = GetParam();
    std::cout << "Test with " << num_threads << " threads." << "\n";
    ThreadPool threadpool(num_threads, num_threads);
    for (int i = 0; i < kLoopCount; ++i)
        threadpool.AddTask(DoNothong);
    // threadpool.WaitForIdle();
}

INSTANTIATE_TEST_CASE_P(ThreadPoolTest, ThreadPoolTest, testing::Values(1, 2, 4, 8));

static void LatencyProc(void* p, int64_t issue_time)
{
    int64_t handle_time = RealtimeClock.MicroSeconds();
    int64_t latency = handle_time - issue_time;
    AtomicAdd(static_cast<int64_t*>(p), latency);
}

static void LatencyTest(int loop_count, bool slow_add)
{
    int64_t total_latency = 0;
    int64_t total_add_latency = 0;
    {
        ThreadPool threadpool;
        for (int i = 0; i < loop_count; ++i)
        {
            int64_t t0 = RealtimeClock.MicroSeconds();
            threadpool.AddTask(std::bind(LatencyProc, &total_latency, t0));
            int64_t t1 = RealtimeClock.MicroSeconds();
            total_add_latency += t1 - t0;
            if (slow_add)
                ThisThread::Sleep(0);
        }
    }
    printf("average latency = %g\n", 1.0 * total_latency / loop_count);
    printf("average add latency = %g\n", 1.0 * total_add_latency / loop_count);
}

TEST(ThreadPool, FastAddLatency)
{
    LatencyTest(100000, false);
}

TEST(ThreadPool, SlowAddLatency)
{
    LatencyTest(100, true);
}

TEST(ThreadPool, CreateDestroyPerformance)
{
    for (int i = 0; i < 100; ++i)
    {
        ThreadPool threadpool;
        threadpool.AddTask(DoNothong);
        threadpool.Terminate(true);
    }
}

} // namespace toft
