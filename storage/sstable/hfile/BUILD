# Copyright (c) 2013, The Toft Authors. All rights reserved.
# Author: Ye Shunping <yeshunping@gmail.com>

cc_library(
    name = 'coding',
    srcs = 'coding.cpp',
    deps = [
        '//toft/encoding:varint',
        '//thirdparty/glog:glog',
    ],
)

cc_test(
    name = 'coding_test',
    srcs = 'coding_test.cpp',
    deps = [':coding'],
)

cc_library(
    name = 'block',
    srcs = 'block.cpp',
    deps = ['//toft/storage/file:file',],
)

cc_library(
    name = 'file_trailer',
    srcs = 'file_trailer.cpp',
    deps = [
        ':block',
        ':coding',
    ],
)

cc_library(
    name = 'file_info',
    srcs = 'file_info.cpp',
    deps = [
        ':block',
        ':coding',
    ],
)

cc_library(
    name = 'data_index',
    srcs = 'data_index.cpp',
    deps = [
        ':block',
        ':coding',
    ],
)

cc_library(
    name = 'data_block',
    srcs = 'data_block.cpp',
    deps = [
        ':block',
        ':coding',
        '//toft/compress/block:block',
    ],
)
