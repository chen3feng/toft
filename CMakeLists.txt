include(CheckIncludeFileCXX)
check_include_file_cxx(type_traits _STD_HAS_TYPE_TRAITS)
check_include_file_cxx(tr1/type_traits _TR1_HAS_TYPE_TRAITS)

configure_file (
    "config.h.in"
    "config.h"
    )

include(toft)
cc_library(toft SRCS ${toft_src} DEPS toft_other hdfs libgsasl libxml2 krb gtest glog gflags cityhash libev boost protobuf snappy lzo uuid krb5_db2 krb5_otp krb5_k5tls krb5_pkinit krb5_test kadm5clint kadm5srv kdb5 gssrpc ssapi_krb5 krad krb5 com_err k5crypto krb5support)
cc_library(toft_other SRCS ${toft_other_src} DEPS hdfs libgsasl libxml2 krb gtest glog gflags cityhash libev boost protobuf snappy lzo uuid krb5_db2 krb5_otp krb5_k5tls krb5_pkinit krb5_test kadm5clint kadm5srv kdb5 gssrpc ssapi_krb5 krad krb5 com_err k5crypto krb5support)
target_link_libraries(toft -lz -lcrypto -liconv -llzma -lgcrypt -lidn -lresolv)
