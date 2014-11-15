// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_INTRUSIVE_LIST_H
#define TOFT_BASE_INTRUSIVE_LIST_H

#include <assert.h>
#include <cstddef>
#include <iterator>

namespace toft {

struct list_node
{
    list_node* prev;
    list_node* next;

public:
    // copy to another node is meaningless, so just init self
    list_node(const list_node& src)
    {
        init();
    }

    list_node()
    {
        init();
    }

    ~list_node()
    {
        unlink();
    }

    void unlink()
    {
        next->prev = prev;
        prev->next = next;
        init();
    }

    void init()
    {
        prev = this;
        next = this;
    }

    /// @return whether is linked into a list
    bool is_linked() const
    {
        return prev != this || next != this;
    }

    // copy to another node is meaningless, so just init self
    list_node& operator=(const list_node&)
    {
        return *this;
    }
};

/// convert member's address to its container's address
template <typename T, typename MemberType>
T* member_to_container(MemberType* member, MemberType T::*member_ptr)
{
    const size_t base_adddress = 0x1000;
    T*p = reinterpret_cast<T*>(base_adddress);
    size_t offset = (size_t)&(p->*member_ptr) - base_adddress;
    return reinterpret_cast<T*>((size_t) member - offset);
}

template <typename T, typename MemberType>
const T* member_to_container(const MemberType* member, MemberType T::*member_ptr)
{
    const size_t base_adddress = 0x1000;
    T*p = reinterpret_cast<T*>(base_adddress);
    size_t offset = (size_t)&(p->*member_ptr) - base_adddress;
    return reinterpret_cast<T*>((size_t) member - offset);
}

template <typename T, list_node T::*Node = &T::link>
class intrusive_list
{
public:
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef const T& const_reference;
    typedef const T* const_pointer;
    typedef std::size_t                                     size_type;
    typedef std::ptrdiff_t                                  difference_type;

    class iterator : public std::iterator<std::bidirectional_iterator_tag, T>
    {
        friend class intrusive_list<T, Node>;
    public:
        iterator() : m_node(NULL) {}

    private:
        explicit iterator(list_node* node) : m_node(node) {}

    public:
        T& operator*() const
        {
            return *member_to_container(m_node, Node);
        }

        T* operator->() const
        {
            return member_to_container(m_node, Node);
        }

        iterator& operator++()
        {
            m_node = m_node->next;
            return *this;
        }

        iterator operator++(int)
        {
            iterator i = *this;
            ++*this;
            return i;
        }

        iterator& operator--()
        {
            m_node = m_node->prev;
            return *this;
        }

        iterator& operator--(int)
        {
            iterator i = *this;
            --*this;
            return i;
        }

        bool operator==(const iterator& rhs) const
        {
            return m_node == rhs.m_node;
        }

        bool operator!=(const iterator& rhs) const
        {
            return m_node != rhs.m_node;
        }

    private:
        list_node* m_node;
    };

    class const_iterator : public std::iterator<std::bidirectional_iterator_tag, const T>
    {
        friend class intrusive_list<T, Node>;
    public:
        const_iterator() : m_node(NULL) {}
    private:
        explicit const_iterator(const list_node* node) : m_node(node) {}
    public:
        const T& operator*() const
        {
            return *member_to_container(m_node, Node);
        }
        const T* operator->() const
        {
            return member_to_container(m_node, Node);
        }

        const_iterator& operator++()
        {
            m_node = m_node->next;
            return *this;
        }
        const_iterator& operator++(int)
        {
            const_iterator i = *this;
            ++*this;
            return i;
        }
        const_iterator& operator--()
        {
            m_node = m_node->prev;
            return *this;
        }
        const_iterator& operator--(int)
        {
            const_iterator i = *this;
            --*this;
            return i;
        }
        bool operator==(const const_iterator& rhs) const
        {
            return m_node == rhs.m_node;
        }
        bool operator!=(const const_iterator& rhs) const
        {
            return m_node != rhs.m_node;
        }

    private:
        const list_node* m_node;
    };

    typedef std::reverse_iterator<iterator>            reverse_iterator;
    typedef std::reverse_iterator<const_iterator>      const_reverse_iterator;

public:
    bool empty() const
    {
        const list_node *next = m_head.next;
        return (next == &m_head) && (next == m_head.prev);
    }

    size_type size() const { return std::distance(begin(), end()); }

    iterator begin() { return iterator(m_head.next); }
    const_iterator begin() const { return const_iterator(m_head.next); }
    iterator end() { return iterator(&m_head); }
    const_iterator end() const { return const_iterator(&m_head); }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    T& front()
    {
        assert(!empty());
        return node_to_value(m_head.next);
    }

    const T& front() const
    {
        assert(!empty());
        return node_to_value(m_head.next);
    }

    T& back()
    {
        assert(!empty());
        return node_to_value(m_head.prev);
    }

    const T& back() const
    {
        assert(!empty());
        return node_to_value(m_head.prev);
    }

    void push_front(T* element)
    {
        add(&(element->*Node), &m_head, m_head.next);
    }
    void push_back(T* element)
    {
        add(&(element->*Node), m_head.prev, &m_head);
    }

    void pop_front()
    {
        assert(!empty());
        erase(&front());
    }
    void pop_back()
    {
        assert(!empty());
        erase(&back());
    }

    static void erase(iterator it)
    {
        it.m_node->unlink();
    }
    static void erase(T* node)
    {
        (node->*Node).unlink();
    }

    void clear()
    {
        iterator end_iter = end();
        for (iterator i = begin(); i != end_iter;)
        {
            erase(i++);
        }
    }

    void splice(intrusive_list& other) {
        if (other.empty())
            return;
        list_node* first = other.m_head.next;
        list_node* last = other.m_head.prev;
        list_node* at = m_head.next;

        first->prev = &m_head;
        m_head.next = first;

        last->next = at;
        at->prev = last;
        other.m_head.init();
    }

private:
    static void add(
        list_node * node,
        list_node *prev,
        list_node *next)
    {
        next->prev = node;
        node->next = next;
        node->prev = prev;
        prev->next = node;
    }

    static T& node_to_value(list_node* node)
    {
        return *member_to_container(node, Node);
    }

    static const T& node_to_value(const list_node* node)
    {
        return *member_to_container(node, Node);
    }

private:
    list_node m_head;
};

} // namespace toft

#endif // TOFT_BASE_INTRUSIVE_LIST_H
