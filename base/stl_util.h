// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// STL utility functions.  Usually, these replace built-in, but slow(!),
// STL functions with more efficient versions.

#ifndef TOFT_BASE_STL_UTIL_H_
#define TOFT_BASE_STL_UTIL_H_

#include <stddef.h>  // for NULL
#include <string.h>  // for memcpy
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace toft {

// Clear internal memory of an STL object.
// STL clear()/reserve(0) does not always free internal memory allocated
// This function uses swap/destructor to ensure the internal memory is freed.
template<class T> void ClearObject(T* obj) {
    T tmp;
    tmp.swap(*obj);
    obj->reserve(0);  // this is because sometimes "T tmp" allocates objects with
    // memory (arena implementation?).  use reserve()
    // to clear() even if it doesn't always work
}

// Reduce memory usage on behalf of object if it is using more than
// "bytes" bytes of space.  By default, we clear objects over 1MB.
template <class T> inline void ClearIfBig(T* obj, size_t limit = 1<<20) {
    if (obj->capacity() >= limit) {
        ClearObject(obj);
    } else {
        obj->clear();
    }
}

// Reserve space for STL object.
// STL's reserve() will always copy.
// This function avoid the copy if we already have capacity
template<class T> void ReserveIfNeeded(T* obj, int new_size) {
    if (obj->capacity() < new_size)   // increase capacity
        obj->reserve(new_size);
    else if (obj->size() > new_size)  // reduce size
        obj->resize(new_size);
}

// DeleteContainerPointers()
//  For a range within a container of pointers, calls delete
//  (non-array version) on these pointers.
// NOTE: for these three functions, we could just implement a DeleteObject
// functor and then call for_each() on the range and functor, but this
// requires us to pull in all of algorithm.h, which seems expensive.
// For hash_[multi]set, it is important that this deletes behind the iterator
// because the hash_set may call the hash function on the iterator when it is
// advanced, which could result in the hash function trying to deference a
// stale pointer.
template <class ForwardIterator>
void DeleteContainerPointers(ForwardIterator begin, ForwardIterator end) {
    while (begin != end) {
        ForwardIterator temp = begin;
        ++begin;
        delete *temp;
    }
}

// DeleteContainerPairPointers()
//  For a range within a container of pairs, calls delete
//  (non-array version) on BOTH items in the pairs.
// NOTE: Like DeleteContainerPointers, it is important that this deletes
// behind the iterator because if both the key and value are deleted, the
// container may call the hash function on the iterator when it is advanced,
// which could result in the hash function trying to dereference a stale
// pointer.
template <class ForwardIterator>
void DeleteContainerPairPointers(ForwardIterator begin,
                                 ForwardIterator end) {
    while (begin != end) {
        ForwardIterator temp = begin;
        ++begin;
        delete temp->first;
        delete temp->second;
    }
}

// DeleteContainerPairFirstPointers()
//  For a range within a container of pairs, calls delete (non-array version)
//  on the FIRST item in the pairs.
// NOTE: Like DeleteContainerPointers, deleting behind the iterator.
template <class ForwardIterator>
void DeleteContainerPairFirstPointers(ForwardIterator begin,
                                      ForwardIterator end) {
    while (begin != end) {
        ForwardIterator temp = begin;
        ++begin;
        delete temp->first;
    }
}

// DeleteContainerPairSecondPointers()
//  For a range within a container of pairs, calls delete
//  (non-array version) on the SECOND item in the pairs.
template <class ForwardIterator>
void DeleteContainerPairSecondPointers(ForwardIterator begin,
                                       ForwardIterator end) {
    while (begin != end) {
        delete begin->second;
        ++begin;
    }
}

// Append elements to the end of vector.

template<typename T, typename A, typename I>
void AppendToVector(std::vector<T, A>* v, I first, I last) {
    v->insert(v.end(), first, last);
}

template<typename T, typename A>
void AppendToVector(std::vector<T, A>* v, const T* buf, size_t size) {
    v->insert(v.end(), buf, buf + size);
}

// To treat a possibly-empty vector as an array, use these functions.
// If you know the array will never be empty, you can use &*v.begin()
// directly, but that is allowed to dump core if v is empty.  This
// function is the most efficient code that will work, taking into
// account how our STL is actually implemented.  THIS IS NON-PORTABLE
// CODE, so call us instead of repeating the nonportable code
// everywhere.  If our STL implementation changes, we will need to
// change this as well.

template<typename T>
inline T* VectorAsArray(std::vector<T>* v) {
# ifdef NDEBUG
    return &*v->begin();
# else
    return v->empty() ? NULL : &*v->begin();
# endif
}

template<typename T>
inline const T* VectorAsArray(const std::vector<T>* v) {
# ifdef NDEBUG
    return &*v->begin();
# else
    return v->empty() ? NULL : &*v->begin();
# endif
}

// Return a mutable char* pointing to a string's internal buffer,
// which may not be null-terminated. Writing through this pointer will
// modify the string.
//
// StringAsArray(&str)[i] is valid for 0 <= i < str.size() until the
// next call to a string method that invalidates iterators.
//
// As of 2006-04, there is no standard-blessed way of getting a
// mutable reference to a string's internal buffer. However, issue 530
// (http://www.open-std.org/JTC1/SC22/WG21/docs/lwg-active.html#530)
// proposes this as the method. According to Matt Austern, this should
// already work on all current implementations.
inline char* StringAsArray(std::string* str) {
    // DO NOT USE const_cast<char*>(str->data())! See the unittest for why.
    return str->empty() ? NULL : &*str->begin();
}

// These are methods that test two hash maps/sets for equality.  These exist
// because the == operator in the STL can return false when the maps/sets
// contain identical elements.  This is because it compares the internal hash
// tables which may be different if the order of insertions and deletions
// differed.

template <class HashSet>
inline bool HashSetEquality(const HashSet& set_a, const HashSet& set_b) {
    if (set_a.size() != set_b.size()) return false;
    for (typename HashSet::const_iterator i = set_a.begin();
         i != set_a.end(); ++i) {
        if (set_b.find(*i) == set_b.end())
            return false;
    }
    return true;
}

template <class HashMap>
inline bool HashMapEquality(const HashMap& map_a, const HashMap& map_b) {
    if (map_a.size() != map_b.size()) return false;
    for (typename HashMap::const_iterator i = map_a.begin();
         i != map_a.end(); ++i) {
        typename HashMap::const_iterator j = map_b.find(i->first);
        if (j == map_b.end()) return false;
        if (i->second != j->second) return false;
    }
    return true;
}

// The following functions are useful for cleaning up STL containers
// whose elements point to allocated memory.

// DeleteElements() deletes all the elements in an STL container and clears
// the container.  This function is suitable for use with a vector, set,
// hash_set, or any other STL container which defines sensible begin(), end(),
// and clear() methods.
//
// If container is NULL, this function is a no-op.
//
// As an alternative to calling DeleteElements() directly, consider
// ElementDeleter (defined below), which ensures that your container's
// elements are deleted when the ElementDeleter goes out of scope.
template <class T>
void DeleteElements(T *container) {
    if (!container) return;
    DeleteContainerPointers(container->begin(), container->end());
    container->clear();
}

// Given an STL container consisting of (key, value) pairs, DeleteValues
// deletes all the "value" components and clears the container.  Does nothing
// in the case it's given a NULL pointer.
template <class T>
void DeleteValues(T *v) {
    if (!v) return;
    for (typename T::iterator i = v->begin(); i != v->end(); ++i) {
        delete i->second;
    }
    v->clear();
}

// The following classes provide a convenient way to delete all elements or
// values from STL containers when they goes out of scope.  This greatly
// simplifies code that creates temporary objects and has multiple return
// statements.  Example:
//
// vector<MyProto *> tmp_proto;
// ElementDeleter<vector<MyProto *> > d(&tmp_proto);
// if (...) return false;
// ...
// return success;

// Given a pointer to an STL container this class will delete all the element
// pointers when it goes out of scope.

template<class Container> class ElementDeleter {
public:
    ElementDeleter<Container>(Container *ptr) : container_ptr_(ptr) {}
    ~ElementDeleter<Container>() { DeleteElements(container_ptr_); }
private:
    Container *container_ptr_;
};

// Given a pointer to an STL container this class will delete all the value
// pointers when it goes out of scope.
template<class Container> class ValueDeleter {
public:
    ValueDeleter<Container>(Container *ptr) : container_ptr_(ptr) {}
    ~ValueDeleter<Container>() { DeleteValues(container_ptr_); }
private:
    Container *container_ptr_;
};

// Translates a set into a vector.
template<typename T>
void SetToVector(const std::set<T>& values, std::vector<T>* result) {
    result->reserve(values.size());
    result->assign(values.begin(), values.end());
}

// Test to see if a set, map, hash_set or hash_map contains a particular key.
// Returns true if the key is in the collection.
// Check if the key is in the container.
template<typename T>
bool HasKey(const T& container, const typename T::key_type& k) {
    return container.find(k) != container.end();
}

// Find a value by a key.
// Return default_value if key is not found.
template<typename T>
const typename T::mapped_type& FindOrDefault(
        const T& container, const typename T::key_type& k,
        const typename T::mapped_type& default_value) {
    typename T::const_iterator it = container.find(k);
    if (it == container.end())
        return default_value;
    return it->second;
}

// Find a pointer to the value of a key.
// Return NULL if key is not found.
template<typename T>
typename T::mapped_type* FindOrNull(
        T& container, const typename T::key_type& k) {
    typename T::iterator it = container.find(k);
    if (it == container.end())
        return NULL;
    return &it->second;
}

// Same as the above FindOrNull, works for const container.
template<typename T>
const typename T::mapped_type* FindOrNull(
        const T& container, const typename T::key_type& k) {
    typename T::const_iterator it = container.find(k);
    if (it == container.end())
        return NULL;
    return &it->second;
}

// In cases that the value in the container is a pointer type, find the pointer
// value of a key.
// Return NULL if key is not found.
template<typename T>
typename T::mapped_type FindPtrOrNull(
        T& container, const typename T::key_type& k) {
    typename T::iterator it = container.find(k);
    if (it == container.end())
        return NULL;
    return it->second;
}

// Same as the above FindPtrOrNull, works for const container.
template<typename T>
const typename T::mapped_type FindPtrOrNull(
        const T& container, const typename T::key_type& k) {
    typename T::const_iterator it = container.find(k);
    if (it == container.end())
        return NULL;
    return it->second;
}

// Finds the value associated with the given key and copies it to *value.
// Returns false if the key was not found, true otherwise.
template <class Collection, class Key, class Value>
bool FindAndCopyTo(const Collection& collection,
                   const Key& key,
                   Value* const value) {
    typename Collection::const_iterator it = collection.find(key);
    if (it == collection.end()) {
        return false;
    }
    *value = it->second;
    return true;
}

// Returns true iff the given collection contains the given key-value pair.
template <class Collection, class Key, class Value>
bool ContainsKeyValuePair(const Collection& collection,
                          const Key& key,
                          const Value& value) {
    typedef typename Collection::const_iterator const_iterator;
    std::pair<const_iterator, const_iterator> range = collection.equal_range(key);
    for (const_iterator it = range.first; it != range.second; ++it) {
        if (it->second == value) {
            return true;
        }
    }
    return false;
}

// Insert a key value pair into the container, return true if this is a new key
// return false otherwise.
template<typename T>
bool InsertIfNotPresent(
        T* container,
        const typename T::key_type& k,
        const typename T::mapped_type& v) {
    return container->insert(std::make_pair(k, v)).second;
}

// Insert a key value pair into the container, replace the existing value for
// the key if exists.
// Return true if this is a new key, return false otherwise.
// If false returned and old is not NULL, *old contains original value for the
// key.
template<typename T>
bool InsertOrReplace(
        T* container,
        const typename T::key_type& k,
        const typename T::mapped_type& v,
        typename T::mapped_type* old = NULL) {
    std::pair<typename T::iterator, bool> ret =
        container->insert(std::make_pair(k, v));
    if (ret.second) {
        return true;
    } else {
        if (old)
            *old = ret.first->second;
        ret.first->second = v;
        return false;
    }
}

}  // namespace toft

#endif  // TOFT_BASE_STL_UTIL_H_
