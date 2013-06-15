// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 04/29/11
// Description: singleton class template

#ifndef TOFT_BASE_SINGLETON_H
#define TOFT_BASE_SINGLETON_H

#include <stddef.h>

#include "toft/base/uncopyable.h"

namespace toft {

// To be used as base template for class to make it singleton
//
// Example: define a singleton class
// class TestClass : public SingletonBase<TestClass>
// {
//     friend class SingletonBase<TestClass>;
// private:
//     TestClass() {}
//     ~TestClass() {}
// public:
//     int Test() const
//     {
//         return 1;
//     }
// };
//
// Example2: define a singleton class with alt access method
// private inherit make 'Instance' method unaccessable
// class TestClass2 : private SingletonBase<TestClass2>
// {
//     friend class SingletonBase<TestClass2>;
// private:
//     TestClass() {}
//     ~TestClass() {}
// public:
//     // using DefaultInstance to instead Instance
//     static TestClass2* DefaultInstance()
//     {
//         return Instance();
//     }
// };
//
template <typename T>
class SingletonBase
{
    TOFT_DECLARE_UNCOPYABLE(SingletonBase);

private:
    // Used to check destructed of object.
    struct Holder
    {
        T value;
        bool is_alive;
        explicit Holder(Holder** holder) : value(), is_alive(true)
        {
            *holder = this;
        }

        template <typename A1>
        Holder(Holder** holder, const A1& a1) : value(a1), is_alive(true)
        {
            *holder = this;
        }


        ~Holder()
        {
            is_alive = false;
        }
    };

protected:
    SingletonBase() {}
    ~SingletonBase() {}

public:
    // Default constructed instance.
    static T* Instance()
    {
        if (!s_holder)
            static Holder holder(&s_holder);
        if (!s_holder->is_alive)
            return NULL;
        return &s_holder->value;
    }

    // Construct singleton with parameter.
    template <typename A1>
    static T* Instance(const A1& a1)
    {
        // Check s_holder before static construct holder to make sure not
        // create another singleton for different parameter.
        if (!s_holder)
            static Holder holder(&s_holder, a1);
        if (!s_holder->is_alive)
            return NULL;
        return &s_holder->value;
    }

    static bool IsAlive()
    {
        return s_holder && s_holder->is_alive;
    }

private:
    static Holder* s_holder;
};

template <typename T>
typename SingletonBase<T>::Holder* SingletonBase<T>::s_holder;

// Make singleton for any existed class.
//
// Example:
// class TestClass3
// {
// };
//
// typedef Singleton<TestClass3> SingletonTestClass3;
// TestClass3* instance = SingletonTestClass3::Instance();
//
template <typename T>
class Singleton : public SingletonBase<T>
{
};

} // namespace toft

#endif // TOFT_BASE_SINGLETON_H
