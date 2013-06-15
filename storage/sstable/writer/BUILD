# Copyright (c) 2013, The Toft Authors. All rights reserved.
# Author: Ye Shunping <yeshunping@gmail.com>

cc_library(
    name = 'base_sstable_writer',
    srcs = 'base_sstable_writer.cpp',
    deps = [
        '//toft/hash:hash',
        '//toft/storage/file:file',
        '//toft/storage/path:path',
        '//toft/compress/block:block',
    ],
)

cc_library(
    name = 'single_sstable_writer',
    srcs = 'single_sstable_writer.cpp',
    deps = [
        ':base_sstable_writer',
        '//toft/storage/sstable:sstable',
    ],
)

cc_library(
    name = 'unsorted_sstable_writer',
    srcs = 'unsorted_sstable_writer.cpp',
    deps = [
        ':base_sstable_writer',
        '//toft/storage/sstable:sstable',
    ],
)

cc_library(
    name = 'composited_sstable_writer',
    srcs = 'composited_sstable_writer.cpp',
    deps = [
        ':base_sstable_writer',
        ':single_sstable_writer',
        ':unsorted_sstable_writer',
        '//toft/storage/sstable:sstable',
        '//toft/storage/sstable:sstable_reader',
    ],
)

cc_library(
    name = 'sharding_sstable_writer',
    srcs = 'sharding_sstable_writer.cpp',
    deps = [
        ':base_sstable_writer',
        ':composited_sstable_writer',
        '//toft/storage/sstable:sstable',
        '//toft/storage/sstable:sstable_reader',
        '//toft/storage/sharding:sharding',
    ],
)