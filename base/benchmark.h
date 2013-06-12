// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>
//  Notes: Idea got from benchmark framework of re2

#ifndef TOFT_BASE_BENCHMARK_H_
#define TOFT_BASE_BENCHMARK_H_

#include <stdint.h>

namespace toft {
struct Benchmark {
    const char* name;
    void (*fn)(int a);
    void (*fnr)(int a, int b);
    int lo;
    int hi;
    int threadlo;
    int threadhi;

    void Register();
    Benchmark(const char* name, void (*f)(int)) {  // NOLINT
        Clear(name);
        fn = f;
        Register();
    }
    Benchmark(const char* name, void (*f)(int, int), int l, int h) {  // NOLINT
        Clear(name);
        fnr = f;
        lo = l;
        hi = h;
        Register();
    }
    void Clear(const char* n) {
        name = n;
        fn = 0;
        fnr = 0;
        lo = 0;
        hi = 0;
        threadlo = 0;
        threadhi = 0;
    }
    Benchmark* ThreadRange(int lo, int hi) {
        threadlo = lo;
        threadhi = hi;
        return this;
    }
};


void SetBenchmarkBytesProcessed(int64_t bytes_processed);
void StopBenchmarkTiming();
void StartBenchmarkTiming();
void BenchmarkMemoryUsage();
void SetBenchmarkItemsProcessed(int n);

void RunBench(toft::Benchmark* b, int nthread, int siz);

extern int nbenchmarks;
extern toft::Benchmark* benchmarks[10000];
}  // namespace toft

//  It's implemented in file: thirdparty/gperftools-2.0/src/base/sysinfo.cc
extern int NumCPUs();

#define TOFT_BENCHMARK(f) \
    ::toft::Benchmark* _benchmark_##f = (new ::toft::Benchmark(#f, f))

#define TOFT_BENCHMARK_RANGE(f, lo, hi) \
    ::toft::Benchmark* _benchmark_##f = \
    (new ::toft::Benchmark(#f, f, lo, hi))

#endif  // TOFT_BASE_BENCHMARK_H_
