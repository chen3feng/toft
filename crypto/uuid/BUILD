cc_library(
    name = 'uuid',
    srcs = 'uuid.cpp',
    deps = ['//toft/base/string:string'],
)

cc_benchmark(
    name = 'uuid_benchmark',
    srcs = 'uuid_benchmark.cpp',
    deps = [':uuid'],
)
