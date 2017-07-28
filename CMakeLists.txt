include(CheckIncludeFileCXX)
check_include_file_cxx(type_traits _STD_HAS_TYPE_TRAITS)
check_include_file_cxx(tr1/type_traits _TR1_HAS_TYPE_TRAITS)

configure_file (
    "config.h.in"
    "config.h"
    )

include(toft)
add_library(toft ${toft_src})
add_dependencies(toft gtest glog gflags cityhash libev boost protobuf snappy lzo)
