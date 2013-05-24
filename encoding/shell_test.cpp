// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-03-30

#include "toft/encoding/shell.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(Shell, Escape)
{
    EXPECT_EQ("Sublime\\ Text\\ 2", ShellEscape("Sublime Text 2"));
}

TEST(Shell, Unescape)
{
    std::string unesacped;
    EXPECT_TRUE(ShellUnescape("Sublime\\ Text\\ 2", &unesacped));
    EXPECT_EQ("Sublime Text 2", unesacped);
}

TEST(Quote, Single)
{
    EXPECT_EQ("'123'", ShellQuote("123", '\''));
    EXPECT_EQ("'12\"3'", ShellQuote("12\"3", '\''));
}

TEST(Quote, Double)
{
    EXPECT_EQ("\"123\"", ShellQuote("123", '"'));
    EXPECT_EQ("\"12'3\"", ShellQuote("12'3", '"'));
    EXPECT_EQ("\"12\\\"3\"", ShellQuote("12\"3", '"'));
}

TEST(Quote, Unquote)
{
    std::string unquoted;

    ASSERT_TRUE(ShellUnquote("123", &unquoted));
    EXPECT_EQ("123", unquoted);

    ASSERT_TRUE(ShellUnquote("\"123\"", &unquoted));
    EXPECT_EQ("123", unquoted);

    ASSERT_TRUE(ShellUnquote("\"12'3\"", &unquoted));
    EXPECT_EQ("12'3", unquoted);

    ASSERT_TRUE(ShellUnquote("\"12\\\"3\"", &unquoted));
    EXPECT_EQ("12\"3", unquoted);

    ASSERT_TRUE(ShellUnquote(" \" 12\\\"3 \" ", &unquoted));
    EXPECT_EQ("  12\"3  ", unquoted);
}

TEST(SplitCommandLine, Normal)
{
    std::vector<std::string> args;
    ASSERT_TRUE(SplitCommandLine("wget a", &args));
    EXPECT_EQ(2U, args.size());
    EXPECT_EQ("wget", args[0]);
    EXPECT_EQ("a", args[1]);
}

TEST(SplitCommandLine, Spaces)
{
    std::vector<std::string> args;
    ASSERT_TRUE(SplitCommandLine(" wget a ", &args));
    EXPECT_EQ(2U, args.size());
    EXPECT_EQ("wget", args[0]);
    EXPECT_EQ("a", args[1]);
}

TEST(SplitCommandLine, SingleQuote)
{
    std::vector<std::string> args;
    ASSERT_TRUE(SplitCommandLine("wget a'b'c", &args));
    EXPECT_EQ(2U, args.size());
    EXPECT_EQ("wget", args[0]);
    EXPECT_EQ("abc", args[1]);
}

TEST(SplitCommandLine, DoubleQuote)
{
    std::vector<std::string> args;
    ASSERT_TRUE(SplitCommandLine("wget a\"b\"c", &args));
    EXPECT_EQ(2U, args.size());
    EXPECT_EQ("wget", args[0]);
    EXPECT_EQ("abc", args[1]);
}

TEST(SplitCommandLine, Escape)
{
    std::vector<std::string> args;
    ASSERT_TRUE(SplitCommandLine("wget a\\ c", &args));
    EXPECT_EQ(2U, args.size());
    EXPECT_EQ("wget", args[0]);
    EXPECT_EQ("a c", args[1]);
}

TEST(Shell, JoinCommandLine)
{
    std::vector<std::string> args;
    args.push_back("wget");
    args.push_back("http://www.qq.com?a=1&b=2");
    EXPECT_EQ("wget \"http://www.qq.com?a=1&b=2\"", JoinCommandLine(args));
}

} // namespace toft
