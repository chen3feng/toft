// Copyright (c) 2012, The Toft Authors. All rights reserved.
// Author: Liu Xiaokang <liuxk02@gmail.com>

#include "toft/storage/path/path.h"
#include <string>
#include <vector>
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(Path, ToAbsolute)
{
    char cwd_buf[4096];
    std::string cwd = getcwd(cwd_buf, sizeof(cwd_buf));
    std::string filepath = "path_test";
    std::string fullpath = cwd + "/" + filepath;
    EXPECT_EQ(fullpath, Path::ToAbsolute(filepath));
    EXPECT_EQ("/d", Path::ToAbsolute("/abc/.././d"));
}

TEST(Path, IsAbsolute)
{
    EXPECT_TRUE(Path::IsAbsolute("/"));
    EXPECT_TRUE(Path::IsAbsolute("/a"));
    EXPECT_FALSE(Path::IsAbsolute("a"));
    EXPECT_FALSE(Path::IsAbsolute("."));
}

TEST(Path, IsRelative)
{
    EXPECT_TRUE(Path::IsRelative("."));
    EXPECT_TRUE(Path::IsRelative("a"));
    EXPECT_FALSE(Path::IsRelative("/a"));
    EXPECT_FALSE(Path::IsRelative("/"));
}

TEST(Path, GetBaseName)
{
    EXPECT_EQ("", Path::GetBaseName("/"));
    EXPECT_EQ("a", Path::GetBaseName("a"));
    EXPECT_EQ("a", Path::GetBaseName("a/"));
    EXPECT_EQ("a.txt", Path::GetBaseName("a.txt"));
    EXPECT_EQ("a.txt", Path::GetBaseName("dir/a.txt"));
    EXPECT_EQ("a", Path::GetBaseName("dir/a"));
}

TEST(Path, GetExtension)
{
    EXPECT_EQ("", Path::GetExtension(""));
    EXPECT_EQ("", Path::GetExtension("/"));
    EXPECT_EQ("", Path::GetExtension("abc"));
    EXPECT_EQ(".txt", Path::GetExtension("abc.txt"));
    EXPECT_EQ(".", Path::GetExtension("abc."));
}

TEST(Path, GetDirectory)
{
    EXPECT_EQ(".", Path::GetDirectory(""));
    EXPECT_EQ("/", Path::GetDirectory("/"));
    EXPECT_EQ("/", Path::GetDirectory("/a"));
    EXPECT_EQ(".", Path::GetDirectory("abc"));
    EXPECT_EQ(".", Path::GetDirectory("abc/"));
    EXPECT_EQ("abc", Path::GetDirectory("abc/d"));
}

TEST(Path, Normalize)
{
    EXPECT_EQ(".", Path::Normalize(""));
    EXPECT_EQ("/", Path::Normalize("///"));
    EXPECT_EQ("//", Path::Normalize("//"));
    EXPECT_EQ("//abc", Path::Normalize("//abc"));
    EXPECT_EQ("/a/b/c", Path::Normalize("///a//b/c//"));
    EXPECT_EQ("../..", Path::Normalize("../../"));
    EXPECT_EQ("../../abc", Path::Normalize("../../abc"));
    EXPECT_EQ("/abc", Path::Normalize("/data/../abc"));
    EXPECT_EQ("/", Path::Normalize("/abc/../../../"));
}

TEST(Path, Join)
{
    EXPECT_EQ("a/b", Path::Join("a", "b"));
    EXPECT_EQ("a/b/c", Path::Join("a", "b", "c"));
    EXPECT_EQ("a/b/c/d", Path::Join("a", "b", "c", "d"));
    EXPECT_EQ("a/b/c/d/e", Path::Join("a", "b", "c", "d", "e"));
    EXPECT_EQ("a/b/c/d/e/f", Path::Join("a", "b/", "c", "d/", "e", "f"));
    EXPECT_EQ("abc/def/", Path::Join("abc", "def/"));
    EXPECT_EQ("/abc/def/", Path::Join("/abc", "def/"));
    EXPECT_EQ("/abc/def/", Path::Join("/abc/", "def/"));
    EXPECT_EQ("/def", Path::Join("/abc/", "/def"));
}

} // namespace toft
