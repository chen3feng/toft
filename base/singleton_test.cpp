// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Description: test singleton

#include "toft/base/singleton.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

// Make a singleton class
class TestClass : public SingletonBase<TestClass>
{
    friend class SingletonBase<TestClass>;
private:
    explicit TestClass() {}
public:
    int Test() const
    {
        return 1;
    }
};

TEST(Singleton, SingletonClass)
{
    TestClass*test = TestClass::Instance();
    EXPECT_EQ(1, test->Test());
}

class TestClass2 : public SingletonBase<TestClass2>
{
    friend class SingletonBase<TestClass2>;
private:
    explicit TestClass2(int value) : m_value(value) {}
public:
    int Test() const
    {
        return m_value;
    }
private:
    int m_value;
};

TEST(Singleton, SingletonClassWithParam)
{
    TestClass2*test = TestClass2::Instance(1);
    EXPECT_EQ(1, test->Test());
}

class TestClass3 {};
TEST(Singleton, ClassSingleton)
{
    TestClass3* test = Singleton<TestClass3>::Instance();
    EXPECT_NE(test, static_cast<TestClass3*>(NULL));
}

class TestClass4 : public SingletonBase<TestClass4>
{
    friend class SingletonBase<TestClass>;
};

TEST(Singleton, IsAlive)
{
    EXPECT_FALSE(TestClass4::IsAlive());
    TestClass4::Instance();
    EXPECT_TRUE(TestClass4::IsAlive());
}

} // namespace toft
