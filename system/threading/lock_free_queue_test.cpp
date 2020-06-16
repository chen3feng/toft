// Copyright (C) 2013, The Toft Authors.
// Author: An Qin <anqin.qin@gmail.com>
//
// Description:

#include "toft/system/threading/lock_free_queue.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(LockFreeQueue, General) {
    LockFreeQueue<int*> my_queue;

    for (int i = 0; i < 10; ++i) {
        int* value = (int*) malloc(sizeof(int));
        *value = i;
        my_queue.Enqueue(value);
    }
    int* value = NULL;
    for (int i = 0; i < 10; ++i) {
        my_queue.Dequeue(value);
        EXPECT_EQ(*value, i);
        delete value;
        value = NULL;
    }
}

} // namespace toft
