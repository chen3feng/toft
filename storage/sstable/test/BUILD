# Copyright (c) 2013, The Toft Authors. All rights reserved.
# Author: Ye Shunping <yeshunping@gmail.com>

cc_test(
    name = 'sstable_test',
    srcs = ['sstable_test.cpp'],
    deps = [
        '//toft/storage/sstable:sstable',
            '//toft/storage/sstable:sstable_writer',
    ],
)

cc_test(
    name = 'sstable_writer_test',
    srcs = ['sstable_writer_test.cpp'],
    deps = ['//toft/storage/sstable:sstable_writer', ]
)

cc_test(
    name = 'composite_sstable_writer_test',
    srcs = ['composite_sstable_writer_test.cpp'],
    deps = ['//toft/storage/sstable:sstable_writer', ]
)

cc_test(
    name = 'merged_sstable_reader_test',
    srcs = ['merged_sstable_reader_test.cpp'],
    deps = [
        '//toft/storage/sstable:sstable_reader',
        '//toft/storage/sstable:sstable_writer',
    ]
)

cc_binary(
    name = 'sstable_display',
    srcs = ['sstable_display.cpp'],
    deps = [
        '//toft/storage/sstable:sstable_writer',
    ]
)
