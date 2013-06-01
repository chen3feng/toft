// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include <stdio.h>

#include <algorithm>

#include "toft/base/benchmark.h"
#include "toft/system/time/clock.h"

#include "thirdparty/glog/logging.h"

namespace {
const char kColorCyan[] = "\033[;36m";
const char kColorPurple[] = "\033[;35m";
const char kColorBlue[] = "\033[;34m";
const char kColorYellow[] = "\033[;33m";
const char kColorGreen[] = "\033[;32m";
const char kColorEnd[] = "\033[0m";
}

namespace toft {

int nbenchmarks;
Benchmark* benchmarks[10000];

void Benchmark::Register() {
    benchmarks[nbenchmarks] = this;
    if (lo < 1) {
        lo = 1;
    }
    if (hi < lo) {
        hi = lo;
    }
    nbenchmarks++;
}

static int64_t processed_bytes;
static int64_t used_time_ns;
static int64_t begin_time_ns;
static int64_t processed_items;

void SetBenchmarkBytesProcessed(int64_t bytes_processed) {
    processed_bytes = bytes_processed;
}

void StopBenchmarkTiming() {
    if (begin_time_ns != 0)
        used_time_ns += RealtimeClock.MicroSeconds() * 1000 - begin_time_ns;
    begin_time_ns = 0;
}

void StartBenchmarkTiming() {
    if (begin_time_ns == 0)
        begin_time_ns = RealtimeClock.MicroSeconds() * 1000;
}

void SetBenchmarkItemsProcessed(int n) {
    processed_items = n;
}

void BenchmarkMemoryUsage() {
    // TODO(yeshunping): Implement it using gperftools
}

static void runN(Benchmark *b, int n, int siz) {
    processed_bytes = 0;
    processed_items = 0;
    used_time_ns = 0;
    begin_time_ns = RealtimeClock.MicroSeconds() * 1000;
    if (b->fn) {
        b->fn(n);
    } else if (b->fnr) {
        b->fnr(n, siz);
    } else {
        fprintf(stderr, "%s: missing function\n", b->name);
        exit(2);
    }
    if (begin_time_ns != 0) {
        used_time_ns += RealtimeClock.MicroSeconds() * 1000 - begin_time_ns;
    }
}

static int round(int n) {
    int base = 1;

    while (base * 10 < n) {
        base *= 10;
    }
    if (n < 2 * base) {
        return 2 * base;
    }
    if (n < 5 * base) {
        return 5 * base;
    }
    return 10 * base;
}

void RunBench(Benchmark* b, int nthread, int siz) {
    int n, last;

    // TODO(yeshunping): support Threaded benchmarks.
    if (nthread != 1) {
        return;
    }

    // run once in case it's expensive
    n = 1;
    runN(b, n, siz);
    while (used_time_ns < static_cast<int>(1e9) &&
           n < static_cast<int>(1e9)) {
        last = n;
        if (used_time_ns / n == 0) {
            n = 1e9;
        } else {
            n = 1e9 / (used_time_ns / n);
        }
        n = std::max(last + 1, std::min(n + n / 2, 100 * last));
        n = round(n);
        runN(b, n, siz);
    }

    char mb[100];
    char suf[100];
    mb[0] = '\0';
    suf[0] = '\0';
    if (used_time_ns > 0 && processed_bytes > 0) {
        snprintf(mb, sizeof mb, "\t%7.2f MB/s",
                 (static_cast<double>(processed_bytes) / 1e6) /
                 (static_cast<double>(used_time_ns) / 1e9));
    }
    if (b->fnr || b->lo != b->hi) {
        if (siz >= (1 << 20)) {
            snprintf(suf, sizeof suf, "/%dM", siz / (1 << 20));
        } else if (siz >= (1 << 10)) {
            snprintf(suf, sizeof suf, "/%dK", siz / (1 << 10));
        } else {
            snprintf(suf, sizeof suf, "/%d", siz);
        }
    }

    printf("%s%s%s%s\t%s%8lld\t%s%10lld ns/op%s%s%s\n",
           kColorCyan, b->name,
           kColorPurple, suf,
           kColorBlue, (long long) n,  // NOLINT
           kColorYellow, (long long) used_time_ns / n,  // NOLINT
           kColorGreen, mb,
           kColorEnd);
    fflush(stdout);
}
}  // namespace toft
