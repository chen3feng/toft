# Copyright (c) 2013, The Toft Authors. All rights reserved.
# Author: Ye Shunping <yeshunping@gmail.com>

cc_library(
    name = '_sstable_reader',
    srcs = [
        'sstable_reader_iterator.cpp',
        'sstable_reader.cpp',
        'sstable_reader_impl.cpp',
        'in_memory_sstable_reader.cpp',
        'on_disk_sstable_reader.cpp',
    ],
    deps = [
        '//toft/storage/sstable:sstable',
        '//toft/base/string:string',
        '//toft/storage/file:file',
        '//toft/system/threading:threading',
        '//toft/compress/block:block',
        '//thirdparty/glog:glog',
    ],
)

cc_library(
    name = '_merged_sstable_reader',
    srcs = [
        'merged_sstable_reader.cpp',
    ],
    deps = [
        ':_sstable_reader',
    ],
)
