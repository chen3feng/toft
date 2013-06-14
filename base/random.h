// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

//  Get code from re2 project

#ifndef TOFT_BASE_RANDOM_H
#define TOFT_BASE_RANDOM_H

#include <stdint.h>

#include "toft/base/uncopyable.h"

namespace toft {

// ACM minimal standard random number generator.  (re-entrant.)
class Random {
    TOFT_DECLARE_UNCOPYABLE(Random);

public:
    explicit Random(int32_t seed) : seed_(seed) {}

    int32_t Next();
    int32_t Uniform(int32_t n);

    void Reset(int32_t seed) {
        seed_ = seed;
    }

    // Randomly returns true ~"1/n" of the time, and false otherwise.
    // REQUIRES: n > 0
    bool OneIn(int n) { return (Next() % n) == 0; }

    // Skewed: pick "base" uniformly from range [0,max_log] and then
    // return "base" random bits.  The effect is to pick a number in the
    // range [0,2^max_log-1] with exponential bias towards smaller numbers.
    uint32_t Skewed(int max_log) {
        return Uniform(1 << Uniform(max_log + 1));
    }

private:
    int32_t seed_;
};

}  // namespace toft

#endif  // TOFT_BASE_RANDOM_H
