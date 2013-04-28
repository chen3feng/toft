// Copyright (c) 2009, The Toft Authors.
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_ATOMIC_TYPE_H
#define TOFT_SYSTEM_ATOMIC_TYPE_H

#include "toft/system/atomic/atomic.h"

namespace toft {

// Atomic integer
template <typename T>
class Atomic
{
    typedef Atomic<T> ThisType;
public:
    Atomic() : m_value()
    {
        EnsureLinkedWithAtomicLibrary();
    }

    // implicit
    Atomic(T value) : m_value(value) // NOLINT(runtime/explicit)
    {
        EnsureLinkedWithAtomicLibrary();
    }

    // generated copy ctor is ok

    operator T() const
    {
        return AtomicGet(&m_value);
    }

    T Value() const
    {
        return AtomicGet(&m_value);
    }

    ThisType& operator=(T value)
    {
        AtomicSet(&m_value, value);
        return *this;
    }

    T operator++()
    {
        return Increment();
    }

    T operator++(int)
    {
        return ExchangeAddWith(1);
    }

    T operator--()
    {
        return Decrement();
    }

    T operator--(int)
    {
        return ExchangeSubWith(1);
    }

    T operator+=(T value)
    {
        return AddWith(value);
    }

    T operator-=(T value)
    {
        return SubWith(value);
    }

    T operator&=(T value)
    {
        return AndWith(value);
    }

    T operator|=(T value)
    {
        return OrWith(value);
    }

    T operator^=(T value)
    {
        return XorWith(value);
    }

public:
    T Exchange(const T value)
    {
        return AtomicExchange(&m_value, value);
    }

    T Increment()
    {
        return AtomicIncrement(&m_value);
    }

    T Decrement()
    {
        return AtomicDecrement(&m_value);
    }

    T ExchangeAddWith(T value)
    {
        return AtomicExchangeAdd(&m_value, value);
    }

    T ExchangeSubWith(T value)
    {
        return AtomicExchangeSub(&m_value, value);
    }

    T ExchangeAndWith(T value)
    {
        return AtomicExchangeAnd(&m_value, value);
    }

    T ExchangeOrWith(T value)
    {
        return AtomicExchangeOr(&m_value, value);
    }

    T ExchangeXorWith(T value)
    {
        return AtomicExchangeXor(&m_value, value);
    }

    T AddWith(T value)
    {
        return AtomicAdd(&m_value, value);
    }

    T SubWith(T value)
    {
        return AtomicSub(&m_value, value);
    }

    T AndWith(T value)
    {
        return AtomicAnd(&m_value, value);
    }

    T OrWith(T value)
    {
        return AtomicOr(&m_value, value);
    }

    T XorWith(T value)
    {
        return AtomicXor(&m_value, value);
    }

    bool CompareExchange(T compare, T exchange, T* old)
    {
        return AtomicCompareExchange(&m_value, compare, exchange, old);
    }

    bool CompareExchange(T compare, T exchange)
    {
        return AtomicCompareExchange(&m_value, compare, exchange);
    }


private:
    T m_value;
};

// Atomic pointer
template <typename T>
class Atomic<T*>
{
    typedef Atomic<T*> ThisType;
public:
    Atomic() : m_value()
    {
    }

    // implicit
    Atomic(T* value) : m_value(value) // NOLINT(runtime/explicit)
    {
    }

    // generated copy ctor is ok

    operator T*() const
    {
        return AtomicGet(&m_value);
    }

    T* Value() const
    {
        return AtomicGet(&m_value);
    }

    ThisType& operator=(T* value)
    {
        m_value = value;
        return *this;
    }

    T* Exchange(T* exchange)
    {
        return AtomicExchange(&m_value, exchange);
    }

    bool CompareExchange(T* compare, T* exchange, T** old)
    {
        return AtomicCompareExchange(&m_value, compare, exchange, old);
    }

private:
    T* m_value;
};

} // namespace toft

#endif // TOFT_SYSTEM_ATOMIC_TYPE_H
