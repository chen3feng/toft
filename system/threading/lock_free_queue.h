// Copyright (C) 2013, The Toft Authors.
// Author: An Qin <anqin.qin@gmail.com>
//
// Description:

#ifndef TOFT_SYSTEM_THREADING_LOCK_FREE_QUEUE_H
#define TOFT_SYSTEM_THREADING_LOCK_FREE_QUEUE_H

#include <stdint.h>
#include <stdlib.h>

namespace toft {

template<typename T, unsigned N = sizeof(uint32_t)>
struct _Pointer {
public:
    union {
        uint64_t ui;
        struct {
            T* ptr;
            size_t count;
        };
    };

    _Pointer() : ptr(NULL), count(0) {}
    _Pointer(T* p) : ptr(p), count(0) {}
    _Pointer(T* p, size_t c) : ptr(p), count(c) {}

    bool cas(_Pointer<T, N> const& nval, _Pointer<T, N> const & cmp) {
        bool result;
        __asm__ __volatile__(
            "lock cmpxchg8b %1\n\t"
            "setz %0\n"
            : "=q" (result), "+m" (ui)
            : "a" (cmp.ptr), "d" (cmp.count), "b" (nval.ptr), "c" (nval.count)
            : "cc"
        );
        return result;
    }

    bool operator==(_Pointer<T,N> const&x) { return x.ui == ui; }
};

template<typename T>
struct _Pointer <T, sizeof(uint64_t)> {
public:
    union {
        uint64_t ui[2];
        struct {
            T* ptr;
            size_t count;
        } __attribute__ (( __aligned__( 16 ) ));
    };

    _Pointer() : ptr(NULL), count(0) {}
    _Pointer(T* p) : ptr(p), count(0) {}
    _Pointer(T* p, size_t c) : ptr(p), count(c) {}

    bool cas(_Pointer<T, 8> const& nval, _Pointer<T, 8> const& cmp) {
        bool result;
        __asm__ __volatile__ (
            "lock cmpxchg16b %1\n\t"
            "setz %0\n"
            : "=q" (result), "+m" (ui)
            : "a" (cmp.ptr), "d" (cmp.count), "b" (nval.ptr), "c" (nval.count)
            : "cc"
        );
        return result;
    }

    bool operator==(_Pointer<T, 8> const&x) {
        return x.ptr == ptr && x.count == count;
    }
};


/////////// lock-free queue ///////////

template<typename T>
class LockFreeQueue {
public:
    struct Node;
    typedef _Pointer<Node, sizeof(size_t)> Pointer;

    struct Node {
        T value;
        Pointer next;

        Node() : next(NULL) {}
        Node(T x, Node* nxt) : value(x), next(nxt) {}
    };

    Pointer m_head, m_tail;

    LockFreeQueue() {
        Node *node = new Node();
        m_head.ptr = m_tail.ptr = node;
    }

    ~LockFreeQueue() {
        Node *node = m_head.ptr;
        m_head.ptr = m_tail.ptr = NULL;
        delete node;
    }

    void Enqueue(T x);
    bool Dequeue(T& pvalue);
};

template<typename T>
void LockFreeQueue<T>::Enqueue(T x) {
    Node *node = new Node(x, NULL);
    Pointer tail, next;
    do {
        tail = m_tail;
        next = tail.ptr->next;
        if (tail == m_tail) {
            if (next.ptr == NULL) {
                if (tail.ptr->next.cas(Pointer(node, next.count + 1), next)) {
                    break;
                }
            } else {
                m_tail.cas(Pointer(next.ptr, tail.count + 1), tail);
            }
        }
    } while (true);
    m_tail.cas(Pointer(node,tail.count + 1), tail);
}

template<typename T>
bool LockFreeQueue<T>::Dequeue(T& pvalue) {
    Pointer head, tail, next;
    do {
        head = m_head;
        tail = m_tail;
        next = head.ptr->next;
        if (head == m_head) {
            if (head.ptr == tail.ptr) {
                if (next.ptr == NULL) return false;
                m_tail.cas(Pointer(next.ptr, tail.count + 1), tail);
            } else {
                pvalue = next.ptr->value;
                if (m_head.cas(Pointer(next.ptr, head.count + 1), head)) {
                    break;
                }
            }
        }
    } while (true);

    delete head.ptr;
    return true;
}

} // namespace toft

#endif  // TOFT_SYSTEM_THREADING_LOCK_FREE_QUEUE_H
