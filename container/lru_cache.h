// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_CONTAINER_LRU_CACHE_H_
#define TOFT_CONTAINER_LRU_CACHE_H_

#include <stdint.h>

#include <list>
#include <map>
#include <utility>

#include "toft/base/uncopyable.h"
#include "toft/system/threading/mutex.h"

#include "thirdparty/glog/logging.h"

namespace toft {

//  It is not thread safe.
template<typename KeyType, typename ValueType, typename LockType = Mutex>
class LruCache {
    TOFT_DECLARE_UNCOPYABLE(LruCache);

public:
    explicit LruCache(size_t capacity);

    ~LruCache();

    // Gets the value from the cache and also update the cache.
    // return NULL if no value found.
    bool Get(const KeyType &key, ValueType* value);

    // Get the value from the cache if exist, return default_value otherwise.
    ValueType GetOrDefault(const KeyType &key,
                           const ValueType& default_value = ValueType());

    // Saves value in cache.
    // If the key already exists, the new value will replace the old one.
    void Put(const KeyType &key, const ValueType& value);

    // Remove by key
    bool Remove(const KeyType &key) {
        typename LockType::Locker locker(&m_mutex);
        return InternalRemove(key);
    }

    bool HasKey(const KeyType& key) const {
        typename LockType::Locker locker(&m_mutex);
        return index_.find(key) != index_.end();
    }

    // Clear all values
    void Clear();

    size_t Size() const {
        typename LockType::Locker locker(&m_mutex);
        return value_list_.size();
    }

    size_t Capacity() const {
        typename LockType::Locker locker(&m_mutex);
        return capacity_;
    }

    bool IsEmpty() const {
        typename LockType::Locker locker(&m_mutex);
        return value_list_.empty();
    }

    bool IsFull() const {
        typename LockType::Locker locker(&m_mutex);
        return index_.size() == capacity_;
    }

private:
    bool InternalRemove(const KeyType &key);

private:
    typedef std::list<std::pair<KeyType, ValueType> > List;
    typedef std::map<KeyType, typename List::iterator> Map;
    mutable LockType m_mutex;
    List value_list_;
    Map index_;
    size_t capacity_;
};

template<typename KeyType, typename ValueType, typename LockType>
LruCache<KeyType, ValueType, LockType>::LruCache(size_t capacity)
                : capacity_(capacity) {
}

template<typename KeyType, typename ValueType, typename LockType>
LruCache<KeyType, ValueType, LockType>::~LruCache() {
    Clear();
}

template<typename KeyType, typename ValueType, typename LockType>
bool LruCache<KeyType, ValueType, LockType>::Get(const KeyType &key, ValueType* value) {
    typename LockType::Locker locker(&m_mutex);
    typename Map::iterator iter = index_.find(key);
    if (iter != index_.end()) {
        value_list_.splice(value_list_.begin(), value_list_, iter->second);

        // Update index.
        iter->second = value_list_.begin();
        *value = iter->second->second;
        return true;
    }
    return false;
}

template<typename KeyType, typename ValueType, typename LockType>
ValueType LruCache<KeyType, ValueType, LockType>::GetOrDefault(
        const KeyType &key,
        const ValueType& default_value) {
    ValueType value;
    if (Get(key, &value))
        return value;
    return default_value;
}

template<typename KeyType, typename ValueType, typename LockType>
void LruCache<KeyType, ValueType, LockType>::Put(const KeyType &key,
                                                 const ValueType& value) {
    typename LockType::Locker locker(&m_mutex);
    {
        // Remove if exists
        typename Map::iterator iter = index_.find(key);
        if (iter != index_.end()) {
            // The inserted value is the same with the one to be deleted.
            if (iter->second->second == value)
                return;
            InternalRemove(key);
        }
    }

    // Insert to list
    value_list_.push_front(std::make_pair(key, value));

    // Update index
    typename List::iterator iter = value_list_.begin();
    index_[key] = iter;

    // Check overflow
    if (index_.size() > capacity_) {
        iter = value_list_.end();
        --iter;
        InternalRemove(iter->first);
    }
}

template<typename KeyType, typename ValueType, typename LockType>
bool LruCache<KeyType, ValueType, LockType>::InternalRemove(const KeyType &key) {
    typename Map::iterator iter = index_.find(key);
    if (iter == index_.end())
        return false;
    value_list_.erase(iter->second);
    index_.erase(iter);
    return true;
}

template<typename KeyType, typename ValueType, typename LockType>
void LruCache<KeyType, ValueType, LockType>::Clear() {
    typename LockType::Locker locker(&m_mutex);
    value_list_.clear();
    index_.clear();
}

}  // namespace toft

#endif  // TOFT_CONTAINER_LRU_CACHE_H_
