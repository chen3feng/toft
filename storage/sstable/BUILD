# Copyright (c) 2013, The Toft Authors. All rights reserved.
# Author: Ye Shunping <yeshunping@gmail.com>

cc_library(
    name = 'sstable',
    deps = [
        '//toft/storage/sstable/hfile:file_trailer',
        '//toft/storage/sstable/hfile:file_info',
        '//toft/storage/sstable/hfile:data_index',
        '//toft/storage/sstable/hfile:data_block',
    ],
)

cc_library(
    name = 'sstable_reader',
    deps = [
        '//toft/storage/sstable/reader:_sstable_reader',
        '//toft/storage/sstable/reader:_merged_sstable_reader',
    ],
)

cc_library(
    name = 'sstable_writer',
    deps = [
        '//toft/storage/sstable/writer:single_sstable_writer',
        '//toft/storage/sstable/writer:unsorted_sstable_writer',
        '//toft/storage/sstable/writer:composited_sstable_writer',
        '//toft/storage/sstable/writer:sharding_sstable_writer',
    ],
)
