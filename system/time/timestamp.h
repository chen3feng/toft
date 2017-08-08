// Copyright 2013, Baidu Inc.
//
// Author: Qianqiong Zhang <zhangqianqiong02@baidu.com>


#ifndef TOFT_SYSTEM_TIME_TIMESTAMP_H_
#define TOFT_SYSTEM_TIME_TIMESTAMP_H_
#pragma once

#include <stdint.h>

namespace toft {

// time stamp in millisecond (1/1000 second)
int64_t GetTimestampInMs();

inline int64_t GetTimestamp() {
    return GetTimestampInMs();
}

// time stamp in microsecond (1/1000000 second)
int64_t GetTimestampInUs();

} // namespace toft

#endif  // TOFT_SYSTEM_TIME_TIMESTAMP_H_
