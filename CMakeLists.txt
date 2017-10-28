include(CheckIncludeFileCXX)
check_include_file_cxx(type_traits _STD_HAS_TYPE_TRAITS)
check_include_file_cxx(tr1/type_traits _TR1_HAS_TYPE_TRAITS)

configure_file (
    "config.h.in"
    "config.h"
    )

include(toft)
cc_library(toft SRCS ${toft_src} DEPS toft_other hdfs gtest glog gflags cityhash libev boost protobuf snappy lzo LibXml2 Kerberos Gsasl Libuuid)
cc_library(toft_other SRCS ${toft_other_src} DEPS  hdfs gtest glog gflags cityhash libev boost protobuf snappy lzo LibXml2 Kerberos Gsasl Libuuid)

