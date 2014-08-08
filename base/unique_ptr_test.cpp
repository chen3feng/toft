// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/unique_ptr.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

class UniquePtrTest : public testing::Test {
protected:
    UniquePtrTest() : p(new int(1)) {}
    std::unique_ptr<int> p;
};

TEST_F(UniquePtrTest, EmptyDeleteOptimize)
{
    EXPECT_EQ(sizeof(p.get()), sizeof(p));
}

TEST_F(UniquePtrTest, Dereference)
{
    EXPECT_EQ(1, *p);
}

TEST_F(UniquePtrTest, Get)
{
    EXPECT_NE(p.get(), static_cast<int*>(NULL));
    EXPECT_EQ(p.get(), p.get());
}

TEST_F(UniquePtrTest, Conversion) {
    EXPECT_TRUE(static_cast<bool>(p));
    p.reset();
    EXPECT_FALSE(static_cast<bool>(p));
}

TEST_F(UniquePtrTest, Release) {
    int* q = p.get();
    int* r = p.release();
    EXPECT_EQ(q, r);
    EXPECT_EQ(NULL, p.get());
    EXPECT_EQ(NULL, p.release());
    delete r;
}

TEST_F(UniquePtrTest, Reset) {
    int* q = new int(1);
    p.reset(q);
    EXPECT_EQ(q, p.get());
}

TEST_F(UniquePtrTest, ResetNull) {
    p.reset();
    EXPECT_EQ(NULL, p.get());
}

TEST(UniqueArrayTest, Dereference) {
    std::unique_ptr<int[]> p(new int[2]());
    ASSERT_EQ(0, p[0]);
    ASSERT_EQ(0, p[1]);
}

} // namespace toft
