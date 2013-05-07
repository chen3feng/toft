cc_library(
    name = 'class_registry',
    deps = './class_registry:_class_registry'
)

cc_library(
    name = 'closure',
    srcs = 'closure.cpp',
)

cc_test(name = 'array_size_test', srcs = 'array_size_test.cpp')
cc_test(name = 'closure_test', srcs = 'closure_test.cpp', deps = ':closure')
cc_test(name = 'functional_test', srcs = 'functional_test.cpp')
cc_test(name = 'intrusive_list_test', srcs = 'intrusive_list_test.cpp')
cc_test(name = 'preprocess_test', srcs = 'preprocess_test.cpp')
cc_test(name = 'shared_ptr_test', srcs = 'shared_ptr_test.cpp')
cc_test(name = 'singleton_test', srcs = 'singleton_test.cpp')
cc_test(name = 'static_assert_test', srcs = 'static_assert_test.cpp')
cc_test(name = 'unique_ptr_test', srcs = 'unique_ptr_test.cpp')

