Get code from chromium project, url for it is:
http://src.chromium.org/svn/trunk/src/net/base/registry_controlled_domains/

We should update below files if them are updated:
effective_tld_names_unittest1.cc
effective_tld_names_unittest2.cc
effective_tld_names.cc
effective_tld_names_unittest1.gperf
effective_tld_names_unittest2.gperf
effective_tld_names.dat
effective_tld_names.gperf

Or we can update effective_tld_names.dat and re-generate other files using gperf