# Copyright 2012. All Rights Reserved.
# Author: shunping_ye@staff.easou.com (Shunping Ye)
#
# Description:
#   TODO(shunpingye): describe this package.

cc_library(name = 'compression',
           deps = [':snappy',
                  ],
          )

cc_library(name = 'snappy',
           srcs = ['snappy.cpp',],
           deps = ['//toft/base:class_registry',
                   '//thirdparty/snappy:snappy',
                   ],
           link_all_symbols=True,
           )

cc_test(name = 'compress_test',
        srcs = [
                'compress_test.cpp',
               ],
        deps = [
                ':compression',
                ],
             )
