// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 10/27/11
// Description:

#include "toft/system/process/sub_process.h"

#include "toft/base/string/algorithm.h"
#include "toft/system/process/this_process.h"
#include "toft/system/threading/this_thread.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

DEFINE_bool(test_mode, true, "run as gtest or normal process");
DEFINE_int32(loop_count, 1, "run loop count");

namespace toft {

class SubProcessTest : public testing::Test
{
protected:
    virtual void SetUp()
    {
        m_exe_path = ThisProcess::BinaryPath();
        m_exe_args = "--test_mode=false";
    }

    virtual void TearDown()
    {
    }

    std::string m_exe_path;
    std::string m_exe_args;
};

TEST_F(SubProcessTest, CreateAndTerminate)
{
    SubProcess process;
    std::string exe_args = "--test_mode=false --loop_count=6";
    ASSERT_TRUE(process.Create(m_exe_path + " " + m_exe_args));
    EXPECT_GT(process.Id(), 0);
    EXPECT_TRUE(process.IsValid());
    EXPECT_TRUE(process.Terminate());
    EXPECT_FALSE(process.IsValid());
}

TEST_F(SubProcessTest, CreateInvalid)
{
    SubProcess process;
    EXPECT_FALSE(process.Create("./non-existed"));
    EXPECT_EQ(0, process.Id());
    EXPECT_FALSE(process.IsValid());
}

TEST_F(SubProcessTest, CreateWithShell)
{
    SubProcess::CreateOptions options;
    options.EnableShell();
    SubProcess process;
    ASSERT_TRUE(process.Create("pwd", options));
    EXPECT_GT(process.Id(), 0);
    EXPECT_TRUE(process.IsValid());
    ASSERT_TRUE(process.WaitForExit());
    EXPECT_FALSE(process.IsValid());
    EXPECT_EQ(0, process.ExitCode());
}

TEST_F(SubProcessTest, CreateWithShellExit)
{
    SubProcess::CreateOptions options;
    options.EnableShell();
    SubProcess process;
    ASSERT_TRUE(process.Create("exit 1", options));
    ASSERT_TRUE(process.WaitForExit());
    EXPECT_EQ(1, process.ExitCode());
}

TEST_F(SubProcessTest, WaitForExit)
{
    SubProcess process;
    ASSERT_TRUE(process.Create(m_exe_path + " " + m_exe_args));
    EXPECT_TRUE(process.WaitForExit());
    EXPECT_EQ(0, process.ExitCode());
    EXPECT_FALSE(process.IsValid());
}

TEST_F(SubProcessTest, TimedWaitForExit)
{
    SubProcess process;
    ASSERT_TRUE(process.Create(m_exe_path + " " + m_exe_args));
    ASSERT_TRUE(process.TimedWaitForExit(6000));
    EXPECT_EQ(0, process.ExitCode());
    EXPECT_FALSE(process.IsValid());
}

TEST_F(SubProcessTest, TimedWaitForExitTimeout)
{
    SubProcess process;
    ASSERT_TRUE(process.Create(m_exe_path + " " + m_exe_args));
    EXPECT_FALSE(process.TimedWaitForExit(100));
    EXPECT_TRUE(process.IsValid());
}

} // namespace toft

static bool g_quit = false;

static void SignalIntHandler(int)
{
    g_quit = true;
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    if (FLAGS_test_mode) {
        return RUN_ALL_TESTS();
    } else {
        signal(SIGINT, SignalIntHandler);
        signal(SIGTERM, SignalIntHandler);

        int count = 0;
        while (!g_quit && count < FLAGS_loop_count) {
            toft::ThisThread::Sleep(500);
            ++count;
        }
        return EXIT_SUCCESS;
    }
}

