// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include "thirdparty/re2/util/benchmark.h"
#include "thirdparty/gflags/gflags.h"

extern int nbenchmarks;
extern testing::Benchmark* benchmarks[];

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, false);
    for (int i = 0; i < nbenchmarks; i++) {
        testing::Benchmark* b = benchmarks[i];
        for (int j = b->threadlo; j <= b->threadhi; j++)
            for (int k = std::max(b->lo, 1); k <= std::max(b->hi, 1); k <<= 1)
                RunBench(b, j, k);
    }
}
