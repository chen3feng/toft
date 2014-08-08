// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_UNIQUE_PTR_H
#define TOFT_BASE_UNIQUE_PTR_H

#include "toft/base/cxx11.h"
#include "toft/base/type_traits.h"

#ifdef TOFT_CXX11_ENABLED

// Using std::unique_ptr
#include <memory>

#else // Non-C++11

#include <assert.h>
#include <stdio.h>
#include <algorithm>

namespace toft {

// Delete pointer deleter, using delete
template <typename T>
struct default_delete
{
    void operator()(T* p)
    {
        delete p;
    }
public:
    typedef char T_must_be_complete_type[sizeof(T) > 0 ? 1 : -1];
};

// Default_delete, specify for array, using delete[]
template <typename T>
struct default_delete<T[]>
{
    void operator()(T* p)
    {
        delete[] p;
    }
private:
    typedef char T_must_be_complete_type[sizeof(T) > 0 ? 1 : -1];
    // Disable polymorphism array delete
    template <typename U> void operator()(U* p) const;
};

// Delete pointer deleter, using delete
template <>
struct default_delete<FILE>
{
    void operator()(FILE* p)
    {
        if (p)
            fclose(p);
    }
};

template <
    typename Type,
    typename Deleter,
    bool DeleterIsEmpty
>
class unique_ptr_base_impl
{
protected:
    unique_ptr_base_impl(Type* ptr, Deleter deleter)
        : m_ptr(ptr), m_deleter(deleter)
    {
    }
    // Get stored deleter
    Deleter& get_deleter()
    {
        return m_deleter;
    }

    // Get stored deleter
    const Deleter& get_deleter() const
    {
        return m_deleter;
    }

    void swap(unique_ptr_base_impl& rhs)
    {
        std::swap(m_ptr, rhs.m_ptr);
        std::swap(m_deleter, rhs.m_deleter);
    }

protected:
    Type* m_ptr;        // Stored pointer
    Deleter m_deleter;    // Stored deleter
};

template <
    typename Type,
    typename Deleter
>
class unique_ptr_base_impl<Type, Deleter, true> : protected Deleter
{
protected:
    unique_ptr_base_impl(Type* ptr, Deleter deleter)
        : Deleter(deleter), m_ptr(ptr)
    {
    }
    // Get stored deleter
    Deleter& get_deleter()
    {
        return *this;
    }

    // Get stored deleter
    const Deleter& get_deleter() const
    {
        return *this;
    }

    void swap(unique_ptr_base_impl& rhs)
    {
        std::swap(m_ptr, rhs.m_ptr);
    }

protected:
    Type* m_ptr;        // Stored pointer
};

// unique_ptr_base, put common members here
template <
    typename Type,
    typename Deleter
>
class unique_ptr_base :
    public unique_ptr_base_impl<Type, Deleter,
        std::is_empty<Deleter>::value>
{
    typedef unique_ptr_base_impl<Type, Deleter,
        std::is_empty<Deleter>::value> base;

public:
    typedef Type element_type;
    typedef Deleter deleter_type;
    typedef Type* pointer;

protected:
    // Default ctor, null pointer initialized
    unique_ptr_base() : base(pointer(), deleter_type())
    {
    }

    // Construct from plain pointer
    explicit unique_ptr_base(pointer ptr) : base(ptr, deleter_type())
    {
    }

    // Construct from plain pointer and deleter
    unique_ptr_base(pointer ptr, Deleter deleter) :
        base(ptr, deleter)
    {
    }

    ~unique_ptr_base()
    {
        if (this->m_ptr)
            this->get_deleter()(this->m_ptr);
#ifndef NDEBUG
        // set to NULL in debug mode to detect wild pointer error
        this->m_ptr = pointer(); // or 0xCFCFCFCFCFCFCFCF or 0xDeadBeefDeadBeef
#endif
    }
public:
    // Assign with plain pointer
    unique_ptr_base& operator=(pointer ptr)
    {
        reset(ptr);
        return *this;
    }

    // Swap with another unique_ptr
    void swap(unique_ptr_base& rhs)
    {
        base::swap(rhs);
    }

    // Release control of stored pointer and return it
    pointer release()
    {
        pointer p = this->m_ptr;
        this->m_ptr = pointer();
        return p;
    }

    // Assigns ptr to the stored pointer
    void reset(pointer ptr = pointer())
    {
        assert(!ptr || ptr != this->m_ptr);
        std::swap(this->m_ptr, ptr);
        if (ptr)
            this->get_deleter()(ptr);
    }

    // Get stored pointer
    pointer get() const
    {
        return this->m_ptr;
    }

    // Safe bool conversion
    operator void* () const
    {
        return this->m_ptr;
    }

    bool operator!() const
    {
        return !this->m_ptr;
    }

private: // disable copy and assign
    unique_ptr_base(const unique_ptr_base& rhs);
    unique_ptr_base& operator=(const unique_ptr_base& rhs);
};

/// unique_ptr for single object
/// @tparam Type object type
/// @tparam Deleter deleter functor
template <
typename Type,
         typename Deleter = default_delete<Type>
         >
class unique_ptr : public unique_ptr_base<Type, Deleter>
{
    typedef unique_ptr_base<Type, Deleter> base;
public:
    typedef typename base::element_type element_type;
    typedef typename base::deleter_type deleter_type;
    typedef typename base::pointer pointer;

public:
    // Make null constructed pointer
    unique_ptr() : base()
    {
    }

    // Construct with a plain pointer
    explicit unique_ptr(pointer ptr) : base(ptr)
    {
    }

    // Construct with a plain pointer and a deleter
    unique_ptr(pointer ptr, Deleter deleter):
        base(ptr, deleter)
    {
    }

    // Assign with plain pointer
    unique_ptr& operator=(pointer ptr)
    {
        base::operator=(ptr);
        return *this;
    }

    // Overloaded operator *
    element_type& operator*() const
    {
        assert(base::get());
        return *base::get();
    }

    // Overloaded operator ->
    typename base::pointer operator->() const
    {
        assert(base::get());
        return base::get();
    }
};

/// unique_ptr for dynamic allocated array
/// @tparam Type object type
/// @tparam Deleter deleter functor
template <
typename Type,
         typename Deleter
         >
class unique_ptr<Type[], Deleter> : public unique_ptr_base<Type, Deleter>
{
    typedef unique_ptr_base<Type, Deleter> base;
public:
    typedef typename base::element_type element_type;
    typedef typename base::deleter_type deleter_type;
    typedef typename base::pointer pointer;

public:
    // Make null constructed pointer
    unique_ptr() { }

    // Construct with a plain pointer
    explicit unique_ptr(pointer ptr) : base(ptr) { }

    // Construct with a plain pointer and a deleter
    unique_ptr(pointer ptr, Deleter deleter):
        base(ptr, deleter)
    {
    }
private: // Disable polymorphism array
    template <typename Type2>
    explicit unique_ptr(Type2 ptr);

    template <typename Type2>
    unique_ptr(Type2 ptr, Deleter deleter);

public:
    // Assign with plain pointer
    unique_ptr& operator=(pointer ptr)
    {
        base::operator=(ptr);
        return *this;
    }

    // Overloaded operator[] for array
    element_type& operator[](size_t index) const
    {
        return base::get()[index];
    }

    // Swap with another unique_ptr
    void swap(unique_ptr& rhs)
    {
        base::swap(rhs);
    }
};

} // namespace toft

namespace std {

using toft::default_delete;
using toft::unique_ptr;

/// specify swap for unique_ptr
template <class Type, class Deleter>
void swap(unique_ptr<Type, Deleter>& lhs, unique_ptr<Type, Deleter>& rhs)
{
    lhs.swap(rhs);
}

template<typename T, typename D, typename U, typename E>
inline bool operator==(const unique_ptr<T, D>& x,
                       const unique_ptr<U, E>& y)
{
    return x.get() == y.get();
}

template<typename T, typename D, typename U, typename E>
inline bool operator!=(const unique_ptr<T, D>& x,
                       const unique_ptr<U, E>& y)
{
    return !(x.get() == y.get());
}

template<typename T, typename D, typename U, typename E>
inline bool operator<(const unique_ptr<T, D>& x,
                      const unique_ptr<U, E>& y)
{
    return x.get() < y.get();
}

template<typename T, typename D, typename U, typename E>
inline bool operator<=(const unique_ptr<T, D>& x,
                       const unique_ptr<U, E>& y)
{
    return !(y.get() < x.get());
}

template<typename T, typename D, typename U, typename E>
inline bool operator>(const unique_ptr<T, D>& x,
                      const unique_ptr<U, E>& y)
{
    return y.get() < x.get();
}

template<typename T, typename D, typename U, typename E>
inline bool operator>=(const unique_ptr<T, D>& x,
                       const unique_ptr<U, E>& y)
{
    return !(x.get() < y.get());
}

} // namespace std

#endif // TOFT_CXX11_ENABLED

#endif // TOFT_BASE_UNIQUE_PTR_H
