include(CheckIncludeFileCXX)
check_include_file_cxx(type_traits _STD_HAS_TYPE_TRAITS)
check_include_file_cxx(tr1/type_traits _TR1_HAS_TYPE_TRAITS)

configure_file (
    "config.h.in"
    "config.h"
    )

include(toft)
cc_library(toft SRCS ${toft_src} DEPS toft_other hdfs libgsasl libxml2 gmock glog gflags cityhash libev boost protobuf snappy stringencoders uuid krb5 k5crypto com_err krb5support)
cc_library(toft_other SRCS ${toft_other_src} DEPS hdfs libgsasl libxml2 gmock glog gflags cityhash libev boost protobuf snappy stringencoders uuid krb5 k5crypto com_err krb5support)

if(APPLE)
  target_link_libraries(toft -lz -lcrypto -liconv -llzma -lgcrypt -lidn -lresolv)
else()
  # looks like keytuils is involed when building krb on Linux platform
  target_link_libraries(toft -lz -lcrypto -liconv -llzma -lgcrypt -lidn -lresolv -lkeyutils)
endif()
