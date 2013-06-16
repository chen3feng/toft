// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_CACHE_LRU_CACHE_H_
#define TOFT_CACHE_LRU_CACHE_H_

#include <stdint.h>

#include <list>
#include <map>
#include <utility>

#include "toft/base/shared_ptr.h"
#include "toft/base/uncopyable.h"

#include "thirdparty/glog/logging.h"

namespace toft {

//  It is not thread safe.
template<typename TypeKey, typename TypeValue>
class LruCache {
    TOFT_DECLARE_UNCOPYABLE(LruCache);

public:
    explicit LruCache(size_t max_size);

    ~LruCache();

    // Gets the value from the cache and also update the cache.
    // return NULL if no value found.
    std::shared_ptr<TypeValue> Get(const TypeKey &key);

    // Saves value in cache.
    // If the key already exists, the new value will replace the old one.
    // The cache will own the value and delete it when finish jobs.
    void Put(const TypeKey &key, TypeValue *value);

    // Saves value in cache.
    void Put(const TypeKey &key, std::shared_ptr<TypeValue> value);

    // Clear all values
    void Clear();

    int Size() const {
        return value_list_.size();
    }

private:
    typedef std::list<std::pair<TypeKey, std::shared_ptr<TypeValue> > > List;
    typedef std::map<TypeKey, typename List::iterator> Map;

    void RemoveValue(const TypeKey &key) {
        typename Map::iterator iter = index_.find(key);
        CHECK(iter != index_.end());

        value_list_.erase(iter->second);
        index_.erase(iter);
    }

    List value_list_;
    Map index_;
    size_t max_size_;
};
template<typename TypeKey, typename TypeValue>
LruCache<TypeKey, TypeValue>::LruCache(size_t max_size)
                : max_size_(max_size) {
}

template<typename TypeKey, typename TypeValue>
LruCache<TypeKey, TypeValue>::~LruCache() {
    Clear();
}

template<typename TypeKey, typename TypeValue>
std::shared_ptr<TypeValue> LruCache<TypeKey, TypeValue>::Get(const TypeKey &key) {
    typename Map::iterator iter = index_.find(key);
    if (iter != index_.end()) {
        value_list_.splice(value_list_.begin(), value_list_, iter->second);

        // Update index.
        iter->second = value_list_.begin();

        return iter->second->second;
    }
    return std::shared_ptr<TypeValue>();
}

template<typename TypeKey, typename TypeValue>
void LruCache<TypeKey, TypeValue>::Put(const TypeKey &key, TypeValue *value) {
    Put(key, std::shared_ptr<TypeValue>(value));
}

template<typename TypeKey, typename TypeValue>
void LruCache<TypeKey, TypeValue>::Put(const TypeKey &key, std::shared_ptr<TypeValue> value) {
    {
        // Remove if exists
        typename Map::iterator iter = index_.find(key);
        if (iter != index_.end()) {
            // The inserted value is the same with the one to be deleted.
            if (value.get() != NULL && iter->second->second.get() == value.get())
                return;
            RemoveValue(key);
        }
    }

    // Insert to list
    value_list_.push_front(std::make_pair(key, value));

    // Update index
    typename List::iterator iter = value_list_.begin();
    index_[key] = iter;

    // Check overflow
    if (index_.size() > max_size_) {
        iter = value_list_.end();
        --iter;
        RemoveValue(iter->first);
    }
}

template<typename TypeKey, typename TypeValue>
void LruCache<TypeKey, TypeValue>::Clear() {
    value_list_.clear();
    index_.clear();
}

}  // namespace toft

#endif  // TOFT_CACHE_LRU_CACHE_H_
