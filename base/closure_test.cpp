// Copyright (c) 2013, The Toft Authors.
// All rights reserved
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/closure.h"

#include <stdio.h>
#include <stdlib.h>

#include "toft/base/functional.h"
#include "toft/base/scoped_ptr.h"
#include "thirdparty/gtest/gtest.h"

// GLOBAL_NOLINT(runtime/int)
// GLOBAL_NOLINT(readability/casting)

namespace toft {

//       preArg
void test0() {
    printf("test0()\r\n");
}
void test1(char preA) {
    printf("test1(),preA=%c\r\n", preA);
}
void test2(char preA, short preB) {
    printf("test2(),preA=%c,preB=%d\r\n", preA, preB);
}
void test3(char preA, short preB, int preC) {
    printf("test3(),preA=%c,preB=%d,preC=%d\r\n", preA, preB, preC);
}

//   preArg_Arg(last one)
void test0_1(char a) {
    printf("test0_1(),a=%c\r\n", a);
}
void test1_1(char preA, short b) {
    printf("test1_1(),preA=%c,b=%d\r\n", preA, b);
}
void test2_1(char preA, short preB, int c) {
    printf("test2_1(),preA=%c,preB=%d,c=%d\r\n", preA, preB, c);
}

//   preArg_Arg(last)
void test0_2(char a, short b) {
    printf("test0_2(),a=%c,b=%d\r\n", a, b);
}
void test1_2(char preA, char a, int b) {
    printf("test1_2(),preA=%c,a=%c,b=%d\n", preA, a, b);
}

//   preArg_Arg(last)
void test0_3(char a, short b, int c) {
    printf("test0_3(),a=%c,b=%d,c=%d\r\n", a, b, c);
}

//----------Closure---------
//       preArg
char Rtest0() {
    printf("Rtest0()\r\n");
    return 'a';
}
char Rtest1(char preA) {
    printf("Rtest1(),preA=%c\r\n", preA);
    return preA;
}
char Rtest2(char preA, short preB) {
    printf("Rtest2(),preA=%c,preB=%d\r\n", preA, preB);
    return preA;
}
char Rtest3(char preA, short preB, int preC) {
    printf("Rtest3(),preA=%c,preB=%d,preC=%d\r\n", preA, preB, preC);
    return preA;
}

//   preArg_Arg(last one)
short Rtest0_1(char a) {
    printf("Rtest0_1(),a=%c\r\n", a);
    return short(980);
}

short Rtest1_1(char preA, short b) {
    printf("Rtest1_1(),preA=%c,b=%d\r\n", preA, b);
    return b;
}

short Rtest2_1(char preA, short preB, int c) {
    printf("Rtest2_1(),preA=%c,preB=%d,c=%d\r\n", preA, preB, c);
    return preB;
}

//   preArg_Arg(last)
int Rtest0_2(char a, short b) {
    printf("Rtest0_2(),a=%c,b=%d\r\n", a, b);
    return int(9900);
}
int Rtest1_2(char preA, char a, int b) {
    printf("Rtest1_2(),preA=%c,a=%c,b=%d\r\n", preA, a, b);
    return b;
}

//   preArg_Arg(last)
unsigned int Rtest0_3(char a, short b, int c) {
    printf("Rtest0_3(),a=%c,b=%d,c=%d\r\n", a, b, c);
    return (unsigned int)c;
}

class TestClass
{
public:
    TestClass() {}
    virtual ~TestClass() {}

    //       preArg
    void test0() {
        printf("test0()\r\n");
    }
    void test1(char preA) {
        printf("test1(),preA=%c\r\n", preA);
    }
    void test2(char preA, short preB) {
        printf("test2(),preA=%c,preB=%d\r\n", preA, preB);
    }
    void test3(char preA, short preB, int preC) {
        printf("test3(),preA=%c,preB=%d,preC=%d\r\n", preA, preB, preC);
    }

    //   preArg_Arg(last one)
    void test0_1(char a) {
        printf("test0_1(),a=%c\r\n", a);
    }
    void test1_1(char preA, short b) {
        printf("test1_1(),preA=%c,b=%d\r\n", preA, b);
    }
    void test2_1(char preA, short preB, int c) {
        printf("test2_1(),preA=%c,preB=%d,c=%d\r\n", preA, preB, c);
    }

    //   preArg_Arg(last)
    void test0_2(char a, short b) {
        printf("test0_2(),a=%c,b=%d\r\n", a, b);
    }
    void test1_2(char preA, char a, int b) {
        printf("test1_2(),preA=%c,a=%c,b=%d\n", preA, a, b);
    }

    //   preArg_Arg(last)
    void test0_3(char a, short b, int c) {
        printf("test0_3(),a=%c,b=%d,c=%d\r\n", a, b, c);
    }

    //----------Closure---------
    //       preArg
    char Rtest0() {
        printf("Rtest0()\r\n");
        return 'a';
    }
    char Rtest1(char preA) {
        printf("Rtest1(),preA=%c\r\n", preA);
        return preA;
    }
    char Rtest2(char preA, short preB) {
        printf("Rtest2(),preA=%c,preB=%d\r\n", preA, preB);
        return preA;
    }
    char Rtest3(char preA, short preB, int preC) {
        printf("Rtest3(),preA=%c,preB=%d,preC=%d\r\n", preA, preB, preC);
        return preA;
    }

    //   preArg_Arg(last one)
    short Rtest0_1(char a) {
        printf("Rtest0_1(),a=%c\r\n", a);
        return short(980);
    }
    short Rtest1_1(char preA, short b) {
        printf("Rtest1_1(),preA=%c,b=%d\r\n", preA, b);
        return b;
    }
    short Rtest2_1(char preA, short preB, int c) {
        printf("Rtest2_1(),preA=%c,preB=%d,c=%d\r\n", preA, preB, c);
        return preB;
    }

    //   preArg_Arg(last)
    int Rtest0_2(char a, short b) {
        printf("Rtest0_2(),a=%c,b=%d\r\n", a, b);
        return int(9900);
    }
    int Rtest1_2(char preA, char a, int b) {
        printf("Rtest1_2(),preA=%c,a=%c,b=%d\r\n", preA, a, b);
        return b;
    }

    //   preArg_Arg(last)
    unsigned int Rtest0_3(char a, short b, int c) {
        printf("Rtest0_3(),a=%c,b=%d,c=%d\r\n", a, b, c);
        return (unsigned int) c;
    }
};

TEST(Closure, Closure)
{
    printf("\r\n==============test NewClosure===============\r\n");

    // Closure0
    Closure<void()> * pcb = NewClosure(test0);
    ASSERT_TRUE(pcb->IsSelfDelete());
    pcb->Run();

    pcb = NewClosure(test1, 'a');
    pcb->Run();

    pcb = NewClosure(test2, char('a'), short(456));
    pcb->Run();

    pcb = NewClosure(test3, char('a'), short(456), int(9800));
    pcb->Run();

    // Closure1
    Closure<void (char)>* pcb0_1 = NewClosure(test0_1);
    pcb0_1->Run('a');

    Closure<void (short)>* pcb1_1 = NewClosure(test1_1, 'a');
    pcb1_1->Run(780);

    Closure<void (int)>* pcb2_1 = NewClosure(test2_1, 'a', 7800);
    pcb2_1->Run(9800);

    // Closure2
    Closure<void (char, short)>* pcb0_2 = NewClosure(test0_2);
    pcb0_2->Run('a', 8900);

    Closure<void (char, int)>* pcb1_2 = NewClosure(test1_2, 'a');
    pcb1_2->Run('b', 780);

    // Closure3
    Closure<void (char, short, int)>* pcb0_3 = NewClosure(test0_3);
    pcb0_3->Run('a', 456, 78909);

    //------Closure------------
    printf("\r\n------Closure------------\r\n");

    // Closure0
    Closure<char ()>* pRcb = NewClosure(Rtest0);
    char r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewClosure(Rtest1, 'a');
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewClosure(Rtest2, 'a', 456);
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewClosure(Rtest3, 'a', 456, 9800);
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    // Closure1
    Closure<short (char)>* pRcb0_1 = NewClosure(Rtest0_1);
    short r1 = pRcb0_1->Run('a');
    EXPECT_EQ(980, r1);

    Closure<short (short)>* pRcb1_1 = NewClosure(Rtest1_1, 'a');
    r1 = pRcb1_1->Run(780);
    EXPECT_EQ(780, r1);

    Closure<short (int)>* pRcb2_1 = NewClosure(Rtest2_1, 'a', 7800);
    r1 = pRcb2_1->Run(9800);
    EXPECT_EQ(7800, r1);

    // Closure2
    Closure<int (char, short)>* pRcb0_2 = NewClosure(Rtest0_2);
    int r2 = pRcb0_2->Run('a', 8900);
    EXPECT_EQ(9900, r2);

    Closure<int (char, int)>* pRcb1_2 = NewClosure(Rtest1_2, 'a');
    r2 = pRcb1_2->Run('b', 780);
    EXPECT_EQ(780, r2);

    // Closure3
    Closure<unsigned int (char, short, int)>* pRcb0_3 = NewClosure(Rtest0_3);
    unsigned int r3 = pRcb0_3->Run('a', 456, 78909);
    EXPECT_EQ(78909U, r3);
}

TEST(Closure, PermanentClosure)
{
    // Closure0
    scoped_ptr<Closure<void()> > pcb(NewPermanentClosure(test0));
    ASSERT_FALSE(pcb->IsSelfDelete());
    pcb->Run();

    pcb.reset(NewPermanentClosure(test1, 'a'));
    pcb->Run();

    pcb.reset(NewPermanentClosure(test2, 'a', 456));
    pcb->Run();

    pcb.reset(NewPermanentClosure(test3, 'a', 456, 9800));
    pcb->Run();

    // Closure1
    scoped_ptr<Closure<void (char)> > pcb0_1(NewPermanentClosure(test0_1));
    pcb0_1->Run('a');

    scoped_ptr<Closure<void (short)> > pcb1_1(NewPermanentClosure(test1_1, 'a'));
    pcb1_1->Run(780);

    scoped_ptr<Closure<void (int)> > pcb2_1(NewPermanentClosure(test2_1, 'a', 7800));
    pcb2_1->Run(9800);

    // Closure2
    scoped_ptr<Closure<void (char, short)> > pcb0_2(NewPermanentClosure(test0_2));
    pcb0_2->Run('a', 8900);

    scoped_ptr<Closure<void (char, int)> > pcb1_2(NewPermanentClosure(test1_2, 'a'));
    pcb1_2->Run('b', 780);

    // Closure3
    scoped_ptr<Closure<void (char, short, int)> > pcb0_3(NewPermanentClosure(test0_3));
    pcb0_3->Run('a', 456, 78909);

    // Closure0
    scoped_ptr<Closure<char ()> > pRcb(NewPermanentClosure(Rtest0));
    char r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb.reset(NewPermanentClosure(Rtest1, 'a'));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb.reset(NewPermanentClosure(Rtest2, 'a', 456));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb.reset(NewPermanentClosure(Rtest3, 'a', 456, 9800));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    // Closure1
    scoped_ptr<Closure<short (char)> > pRcb0_1(NewPermanentClosure(Rtest0_1));
    short r1 = pRcb0_1->Run('a');
    EXPECT_EQ(980, r1);

    scoped_ptr<Closure<short (short)> > pRcb1_1(NewPermanentClosure(Rtest1_1, 'a'));
    r1 = pRcb1_1->Run(780);
    EXPECT_EQ(780, r1);

    scoped_ptr<Closure<short (int)> > pRcb2_1(NewPermanentClosure(Rtest2_1, 'a', 7800));
    r1 = pRcb2_1->Run(9800);
    EXPECT_EQ(7800, r1);

    // Closure2
    scoped_ptr<Closure<int (char, short)> > pRcb0_2(NewPermanentClosure(Rtest0_2));
    int r2 = pRcb0_2->Run('a', 8900);
    EXPECT_EQ(9900, r2);

    scoped_ptr<Closure<int (char, int)> > pRcb1_2(NewPermanentClosure(Rtest1_2, 'a'));
    r2 = pRcb1_2->Run('b', 780);
    EXPECT_EQ(780, r2);

    // Closure3
    scoped_ptr<Closure<unsigned int (char, short, int)> > pRcb0_3(NewPermanentClosure(Rtest0_3));
    unsigned int r3 = pRcb0_3->Run('a', 456, 78909);
    EXPECT_EQ(78909U, r3);
}

TEST(Closure, MethodClosure)
{
    printf("\r\n==============test NewClosure(Class)===============\r\n");

    TestClass obj;

    // Closure0
    Closure<void ()>* pcb = NewClosure(&obj, &TestClass::test0);
    pcb->Run();

    pcb = NewClosure(&obj, &TestClass::test1, 'a');
    pcb->Run();

    pcb = NewClosure(&obj, &TestClass::test2, 'a', 456);
    pcb->Run();

    pcb = NewClosure(&obj, &TestClass::test3, 'a', 456, 9800);
    pcb->Run();

    // Closure1
    Closure<void (char)>* pcb0_1 = NewClosure(&obj, &TestClass::test0_1);
    pcb0_1->Run('a');

    Closure<void (short)>* pcb1_1 = NewClosure(&obj, &TestClass::test1_1, 'a');
    pcb1_1->Run(short(780));

    Closure<void (int)>* pcb2_1 = NewClosure(&obj, &TestClass::test2_1, 'a', 7800);
    pcb2_1->Run(9800);

    // Closure2
    Closure<void (char, short)>* pcb0_2 = NewClosure(&obj, &TestClass::test0_2);
    pcb0_2->Run('a', 8900);

    Closure<void (char, int)>* pcb1_2 = NewClosure(&obj, &TestClass::test1_2, 'a');
    pcb1_2->Run('b', 780);

    // Closure3
    Closure<void (char, short, int)>* pcb0_3 = NewClosure(&obj, &TestClass::test0_3);
    pcb0_3->Run('a', 456, 78909);

    //------Closure------------
    printf("\r\n------Closure------------\r\n");

    // Closure0
    Closure<char ()>* pRcb = NewClosure(&obj, &TestClass::Rtest0);
    char r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewClosure(&obj, &TestClass::Rtest1, 'a');
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewClosure(&obj, &TestClass::Rtest2, 'a', 456);
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb = NewClosure(&obj, &TestClass::Rtest3, 'a', 456, 9800);
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    // Closure1
    Closure<short (char)>* pRcb0_1 = NewClosure(&obj, &TestClass::Rtest0_1);
    short r1 = pRcb0_1->Run('a');
    EXPECT_EQ(980, r1);

    Closure<short (short)>* pRcb1_1 = NewClosure(&obj, &TestClass::Rtest1_1, 'a');
    r1 = pRcb1_1->Run(780);
    EXPECT_EQ(780, r1);

    Closure<short (int)>* pRcb2_1 = NewClosure(&obj, &TestClass::Rtest2_1, 'a', 7800);
    r1 = pRcb2_1->Run(9800);
    EXPECT_EQ(7800, r1);

    // Closure2
    Closure<int (char, short)>* pRcb0_2 =
        NewClosure(&obj, &TestClass::Rtest0_2);
    int r2 = pRcb0_2->Run('a', 8900);
    EXPECT_EQ(9900, r2);

    Closure<int (char, int)>* pRcb1_2 = NewClosure(&obj, &TestClass::Rtest1_2, 'a');
    r2 = pRcb1_2->Run('b', 780);
    EXPECT_EQ(780, r2);

    // Closure3
    Closure<void (char, short, int)>* pRcb0_3 = NewClosure(&obj, &TestClass::test0_3);
    pRcb0_3->Run('a', 456, 78909);
}

TEST(Closure, PermanentMethodClosure)
{
    printf("\r\n==============test NewClosure(Class)===============\r\n");

    TestClass obj;

    // Closure0
    scoped_ptr<Closure<void()> > pcb(NewPermanentClosure(&obj, &TestClass::test0));
    pcb->Run();

    pcb.reset(NewPermanentClosure(&obj, &TestClass::test1, 'a'));
    pcb->Run();

    pcb.reset(NewPermanentClosure(&obj, &TestClass::test2, 'a', 456));
    pcb->Run();

    pcb.reset(NewPermanentClosure(&obj, &TestClass::test3, 'a', 456, 9800));
    pcb->Run();

    scoped_ptr<Closure<void (char)> > pcb0_1(NewPermanentClosure(&obj, &TestClass::test0_1));
    pcb0_1->Run('a');

    scoped_ptr<Closure<void (short)> > pcb1_1(NewPermanentClosure(&obj, &TestClass::test1_1, 'a'));
    pcb1_1->Run(short(780));

    scoped_ptr<Closure<void (int)> > pcb2_1(
        NewPermanentClosure(&obj, &TestClass::test2_1, 'a', 7800));
    pcb2_1->Run(9800);

    // Closure2
    scoped_ptr<Closure<void (char, short)> > pcb0_2(NewPermanentClosure(&obj, &TestClass::test0_2));
    pcb0_2->Run('a', 8900);

    scoped_ptr<Closure<void (char, int)> > pcb1_2(NewPermanentClosure(&obj, &TestClass::test1_2, 'a'));
    pcb1_2->Run('b', 780);

    // Closure3
    scoped_ptr<Closure<void (char, short, int)> > pcb0_3(NewPermanentClosure(&obj, &TestClass::test0_3));
    pcb0_3->Run('a', 456, 78909);

    // Closure0
    scoped_ptr<Closure<char ()> > pRcb(NewPermanentClosure(&obj, &TestClass::Rtest0));
    char r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb.reset(NewPermanentClosure(&obj, &TestClass::Rtest1, 'a'));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb.reset(NewPermanentClosure(&obj, &TestClass::Rtest2, 'a', 456));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    pRcb.reset(NewPermanentClosure(&obj, &TestClass::Rtest3, 'a', 456, 9800));
    r0 = pRcb->Run();
    EXPECT_EQ('a', r0);

    // Closure1
    scoped_ptr<Closure<short (char)> > pRcb0_1(NewPermanentClosure(&obj, &TestClass::Rtest0_1));
    short r1 = pRcb0_1->Run('a');
    EXPECT_EQ(980, r1);

    scoped_ptr<Closure<short (short)> > pRcb1_1(NewPermanentClosure(&obj, &TestClass::Rtest1_1, 'a'));
    r1 = pRcb1_1->Run(780);
    EXPECT_EQ(780, r1);

    scoped_ptr<Closure<short (int)> > pRcb2_1(
        NewPermanentClosure(&obj, &TestClass::Rtest2_1, 'a', 7800));
    r1 = pRcb2_1->Run(9800);
    EXPECT_EQ(7800, r1);

    // Closure2
    scoped_ptr<Closure<int (char, short)> > pRcb0_2(NewPermanentClosure(&obj, &TestClass::Rtest0_2));
    int r2 = pRcb0_2->Run('a', 8900);
    EXPECT_EQ(9900, r2);

    scoped_ptr<Closure<int (char, int)> > pRcb1_2(NewPermanentClosure(&obj, &TestClass::Rtest1_2, 'a'));
    r2 = pRcb1_2->Run('b', 780);
    EXPECT_EQ(780, r2);

    scoped_ptr<Closure<unsigned int (char, short, int)> > pRcb0_3(NewPermanentClosure(&obj, &TestClass::Rtest0_3));
    unsigned int r3 = pRcb0_3->Run('a', 456, 78909);
    EXPECT_EQ(78909U, r3);
}

void NullFunction()
{
}

class Performance : public testing::Test {};

const int kLoopCount = 10000000;

TEST_F(Performance, CxxFunction)
{
    for (int i = 0; i < kLoopCount; ++i)
    {
        NullFunction();
    }
}

TEST_F(Performance, Closure)
{
    for (int i = 0; i < kLoopCount; ++i)
    {
        Closure<void()>* closure = NewClosure(NullFunction);
        closure->Run();
    }
}

TEST_F(Performance, PermantClosure)
{
    scoped_ptr<Closure<void()> > closure(NewPermanentClosure(&NullFunction));
    for (int i = 0; i < kLoopCount; ++i)
    {
        closure->Run();
    }
}

TEST_F(Performance, StdFunction)
{
    std::function<void ()> function = NullFunction;
    for (int i = 0; i < kLoopCount; ++i)
    {
        function();
    }
}

TEST_F(Performance, OnceStdFunction)
{
    for (int i = 0; i < kLoopCount; ++i)
    {
        std::function<void ()> function = NullFunction;
        function();
    }
}

class ClosureInheritTest : public testing::Test
{
public:
    int Foo() { return 42; }
};

TEST_F(ClosureInheritTest, Test)
{
    Closure<int ()>* closure = NewClosure(this, &ClosureInheritTest::Foo);
    EXPECT_EQ(42, closure->Run());
}

#if 0
void RefTest(int& a, const int& b)
{
    printf("a = %d, b = %d\n", a, b);
    a = b;
}

TEST(Closure, Reference)
{
    Closure<void (int&, const int&)>* c1 = NewClosure(&RefTest);
    int a, b = 2;
    c1->Run(a, b);
    printf("a = %d, b = %d\n", a, b);
}
#endif

#if __GXX_EXPERIMENTAL_CXX0X__
TEST(Closure, FunctorClosure)
{
    int n = 0;
    auto task = [&]() {
        ++n;
    };

    auto a = NewClosure(task);
    a->Run();
    EXPECT_EQ(1, n);

    auto b = NewPermanentClosure(task);
    b->Run();
    b->Run();
    b->Run();
    EXPECT_EQ(4, n);
    delete b;
}

TEST(Closure, FunctorClosureWithReturnValue)
{
    int n = 0;
    auto task = [&]()->int {
        return ++n;
    };

    auto a = NewClosure(task);
    EXPECT_EQ(1, a->Run());
    EXPECT_EQ(1, n);
}

#endif

} // namespace toft
