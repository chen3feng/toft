// Copyright (c) 2009, The Toft Authors.
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_ATOMIC_FUNCTIONS_H
#define TOFT_SYSTEM_ATOMIC_FUNCTIONS_H

/*

Function style interface

///////////////////////////////////////////////////////////////////////////////
// return value
AtomicGet

///////////////////////////////////////////////////////////////////////////////
// Change value and return
AtomicSet
AtomicAdd
AtomicSub
AtomicAnd
AtomicOr
AtomicXor

T Atomic<Operation>(T* target, T value)
Operation:
    atomically
    {
        *target operation value;
        return target;
    }

///////////////////////////////////////////////////////////////////////////////
// change value and return old value

AtomicExchangeSet
AtomicExchangeAdd
AtomicExchangeSub
AtomicExchangeAnd
AtomicExchangeOr
AtomicExchangeXor

Prototype:
    T AtomicExchange<Operation>(T* target, T value)

Operation:
    atomically
    {
        T old = *target;
        *target operation value;
        return old;
    }


///////////////////////////////////////////////////////////////////////////////
// compare and change

Prototype:
    bool AtomicCompareExchange(T* value, T compare, T exchange, T* old)

Operation:
    atomically
    {
        old = *value;
        if (*value == compare)
        {
            *value = exchange;
            return true;
        }
        return false;
    }

*/

namespace toft {

#ifdef NDEBUG
inline void EnsureLinkedWithAtomicLibrary() {}
#else
void EnsureLinkedWithAtomicLibrary();
#endif

template <typename T>
T AtomicGet(const T* ptr)
{
    __sync_synchronize();
    return *ptr;
}

template <typename T>
T AtomicSet(T* ptr, T value)
{
    EnsureLinkedWithAtomicLibrary();
    (void)__sync_lock_test_and_set(ptr, value);
    return value;
}

template <typename T>
T AtomicExchange(T* ptr, T value)
{
    return __sync_lock_test_and_set(ptr, value);
}

template <typename T>
T AtomicAdd(T* ptr, T value)
{
    EnsureLinkedWithAtomicLibrary();
    return __sync_add_and_fetch(ptr, value);
}

template <typename T>
T AtomicSub(T* ptr, T value)
{
    return __sync_sub_and_fetch(ptr, value);
}

template <typename T>
T AtomicOr(T* ptr, T value)
{
    return __sync_or_and_fetch(ptr, value);
}

template <typename T>
T AtomicAnd(T* ptr, T value)
{
    return __sync_and_and_fetch(ptr, value);
}

template <typename T>
T AtomicXor(T* ptr, T value)
{
    return __sync_xor_and_fetch(ptr, value);
}

template <typename T>
T AtomicIncrement(T* ptr)
{
    return __sync_add_and_fetch(ptr, 1);
}

template <typename T>
T AtomicDecrement(T* ptr)
{
    return __sync_sub_and_fetch(ptr, 1);
}

template <typename T>
T AtomicExchangeAdd(T* ptr, T value)
{
    return __sync_fetch_and_add(ptr, value);
}

template <typename T>
T AtomicExchangeSub(T* ptr, T value)
{
    return __sync_fetch_and_sub(ptr, value);
}

template <typename T>
T AtomicExchangeOr(T* ptr, T value)
{
    return __sync_fetch_and_or(ptr, value);
}

template <typename T>
T AtomicExchangeAnd(T* ptr, T value)
{
    return __sync_fetch_and_and(ptr, value);
}

template <typename T>
T AtomicExchangeXor(T* ptr, T value)
{
    return __sync_fetch_and_xor(ptr, value);
}

template <typename T>
bool AtomicCompareExchange(T* ptr, T compare, T exchange, T* old)
{
    *old = *ptr;
    if (__sync_bool_compare_and_swap(ptr, compare, exchange))
    {
        *old = compare;
        return true;
    }
    return false;
}

template <typename T>
bool AtomicCompareExchange(T* ptr, T compare, T exchange)
{
    return __sync_bool_compare_and_swap(ptr, compare, exchange);
}

} // namespace toft

#endif // TOFT_SYSTEM_ATOMIC_FUNCTIONS_H
