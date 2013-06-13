// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_CACHE_THREADSAFE_LRU_CACHE_H
#define TOFT_CACHE_THREADSAFE_LRU_CACHE_H

#include <list>
#include <utility>

#include "toft/base/scoped_ptr.h"
#include "toft/base/shared_ptr.h"
#include "toft/cache/lru_cache.h"
#include "toft/system/threading/mutex.h"

namespace toft {

// This is a thread safe LRU cache.
template<typename TypeKey, typename TypeValue>
class ThreadSafeLRUCache {
    TOFT_DECLARE_UNCOPYABLE(ThreadSafeLRUCache);

public:
    explicit ThreadSafeLRUCache(uint32_t max_size);

    ~ThreadSafeLRUCache();

    bool Get(const TypeKey &key, std::shared_ptr<TypeValue> *value);

    void Put(const TypeKey &key, TypeValue *value);
    void Put(const TypeKey &key, std::shared_ptr<TypeValue> value);

    uint32_t Size() const {
        return cache_->Size();
    }

    void Clear();

private:
    toft::scoped_ptr<LRUCache<TypeKey, TypeValue> > cache_;
    toft::Mutex mu_;
};

template<typename TypeKey, typename TypeValue>
ThreadSafeLRUCache<TypeKey, TypeValue>::ThreadSafeLRUCache(uint32_t max_size) {
    toft::MutexLocker lock(&mu_);
    cache_.reset(new LRUCache<TypeKey, TypeValue>(max_size));
}

template<typename TypeKey, typename TypeValue>
ThreadSafeLRUCache<TypeKey, TypeValue>::~ThreadSafeLRUCache() {
    toft::MutexLocker lock(&mu_);
    cache_->Clear();
}

template<typename TypeKey, typename TypeValue>
bool ThreadSafeLRUCache<TypeKey, TypeValue>::Get(const TypeKey &key,
                                                 std::shared_ptr<TypeValue> *value) {
    toft::MutexLocker lock(&mu_);
    *value = cache_->Get(key);
    return value->get() != NULL;
}

template<typename TypeKey, typename TypeValue>
void ThreadSafeLRUCache<TypeKey, TypeValue>::Put(const TypeKey &key, TypeValue *value) {
    toft::MutexLocker lock(&mu_);
    cache_->Put(key, value);
}

template<typename TypeKey, typename TypeValue>
void ThreadSafeLRUCache<TypeKey, TypeValue>::Put(const TypeKey &key,
                                                 std::shared_ptr<TypeValue> value) {
    toft::MutexLocker lock(&mu_);
    cache_->Put(key, value);
}

template<typename TypeKey, typename TypeValue>
void ThreadSafeLRUCache<TypeKey, TypeValue>::Clear() {
    toft::MutexLocker lock(&mu_);
    cache_->Clear();
}
}  // namespace toft

#endif  // TOFT_CACHE_THREADSAFE_LRU_CACHE_H
