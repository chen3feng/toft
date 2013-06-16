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
class ThreadSafeLruCache {
    TOFT_DECLARE_UNCOPYABLE(ThreadSafeLruCache);

public:
    explicit ThreadSafeLruCache(size_t max_size);

    ~ThreadSafeLruCache();

    bool Get(const TypeKey &key, std::shared_ptr<TypeValue> *value);

    void Put(const TypeKey &key, TypeValue *value);
    void Put(const TypeKey &key, std::shared_ptr<TypeValue> value);

    size_t Size() const {
        return cache_->Size();
    }

    void Clear();

private:
    toft::scoped_ptr<LruCache<TypeKey, TypeValue> > cache_;
    toft::Mutex mu_;
};

template<typename TypeKey, typename TypeValue>
ThreadSafeLruCache<TypeKey, TypeValue>::ThreadSafeLruCache(size_t max_size) {
    toft::MutexLocker lock(&mu_);
    cache_.reset(new LruCache<TypeKey, TypeValue>(max_size));
}

template<typename TypeKey, typename TypeValue>
ThreadSafeLruCache<TypeKey, TypeValue>::~ThreadSafeLruCache() {
    toft::MutexLocker lock(&mu_);
    cache_->Clear();
}

template<typename TypeKey, typename TypeValue>
bool ThreadSafeLruCache<TypeKey, TypeValue>::Get(const TypeKey &key,
                                                 std::shared_ptr<TypeValue> *value) {
    toft::MutexLocker lock(&mu_);
    *value = cache_->Get(key);
    return value->get() != NULL;
}

template<typename TypeKey, typename TypeValue>
void ThreadSafeLruCache<TypeKey, TypeValue>::Put(const TypeKey &key, TypeValue *value) {
    toft::MutexLocker lock(&mu_);
    cache_->Put(key, value);
}

template<typename TypeKey, typename TypeValue>
void ThreadSafeLruCache<TypeKey, TypeValue>::Put(const TypeKey &key,
                                                 std::shared_ptr<TypeValue> value) {
    toft::MutexLocker lock(&mu_);
    cache_->Put(key, value);
}

template<typename TypeKey, typename TypeValue>
void ThreadSafeLruCache<TypeKey, TypeValue>::Clear() {
    toft::MutexLocker lock(&mu_);
    cache_->Clear();
}
}  // namespace toft

#endif  // TOFT_CACHE_THREADSAFE_LRU_CACHE_H
