// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "toft/net/domain/domain_util.h"

#include "toft/net/uri/uri.h"

#include "thirdparty/gtest/gtest.h"

#include "effective_tld_names_unittest1.cc"
#undef TOTAL_KEYWORDS
#undef MIN_WORD_LENGTH
#undef MAX_WORD_LENGTH
#undef MIN_HASH_VALUE
#undef MAX_HASH_VALUE
#include "effective_tld_names_unittest2.cc"

namespace toft {

URI GetURI(const std::string& url) {
    URI uri;
    uri.Parse(url);
    return uri;
}

std::string GetDomainFromURL(const std::string& url) {
    return DomainUtil::GetDomainAndRegistry(GetURI(url), EXCLUDE_PRIVATE_REGISTRIES);
}

std::string GetDomainFromHost(const std::string& host) {
    return DomainUtil::GetDomainAndRegistry(host, EXCLUDE_PRIVATE_REGISTRIES);
}

size_t GetRegistryLengthFromURL(
    const std::string& url,
    UnknownRegistryFilter unknown_filter) {
    return DomainUtil::GetRegistryLength(GetURI(url),
                                         unknown_filter,
                                         EXCLUDE_PRIVATE_REGISTRIES);
}

size_t GetRegistryLengthFromURLIncludingPrivate(
    const std::string& url,
    UnknownRegistryFilter unknown_filter) {
    return DomainUtil::GetRegistryLength(GetURI(url),
                                         unknown_filter,
                                         INCLUDE_PRIVATE_REGISTRIES);
}

size_t GetRegistryLengthFromHost(
    const std::string& host,
    UnknownRegistryFilter unknown_filter) {
    return DomainUtil::GetRegistryLength(host, unknown_filter, EXCLUDE_PRIVATE_REGISTRIES);
}

bool CompareDomains(const std::string& url1, const std::string& url2) {
    URI g1 = GetURI(url1);
    URI g2 = GetURI(url2);
    return DomainUtil::SameDomainOrHost(g1, g2, EXCLUDE_PRIVATE_REGISTRIES);
}

class RegistryControlledDomainTest : public testing::Test {
protected:
    void UseDomainData(FindDomainPtr function) {
        SetFindDomainFunctionForTesting(function);
    }

    virtual void TearDown() {
        SetFindDomainFunctionForTesting(NULL);
    }
};

TEST_F(RegistryControlledDomainTest, TestGetDomainAndRegistry) {
    UseDomainData(Perfect_Hash_Test1::FindDomain);

    // Test URI version of GetDomainAndRegistry().
    EXPECT_EQ("baz.jp", GetDomainFromURL("http://a.baz.jp/file.html"));    // 1
//  EXPECT_EQ("baz.jp.", GetDomainFromURL("http://a.baz.jp./file.html"));  // 1
    EXPECT_EQ("", GetDomainFromURL("http://ac.jp"));                       // 2
    EXPECT_EQ("", GetDomainFromURL("http://a.bar.jp"));                    // 3
    EXPECT_EQ("", GetDomainFromURL("http://bar.jp"));                      // 3
    EXPECT_EQ("", GetDomainFromURL("http://baz.bar.jp"));                  // 3 4
    EXPECT_EQ("a.b.baz.bar.jp", GetDomainFromURL("http://a.b.baz.bar.jp"));  // 4
    EXPECT_EQ("pref.bar.jp", GetDomainFromURL("http://baz.pref.bar.jp"));  // 5
//  EXPECT_EQ("b.bar.baz.com.", GetDomainFromURL("http://a.b.bar.baz.com."));  // 6
    EXPECT_EQ("a.d.c", GetDomainFromURL("http://a.d.c"));                  // 7
//  EXPECT_EQ("a.d.c", GetDomainFromURL("http://.a.d.c"));                 // 7
//  EXPECT_EQ("a.d.c", GetDomainFromURL("http://..a.d.c"));                // 7
    EXPECT_EQ("b.c", GetDomainFromURL("http://a.b.c"));                    // 7 8
    EXPECT_EQ("baz.com", GetDomainFromURL("http://baz.com"));              // none
//  EXPECT_EQ("baz.com.", GetDomainFromURL("http://baz.com."));            // none

    EXPECT_EQ("", GetDomainFromURL(std::string()));
    EXPECT_EQ("", GetDomainFromURL("http://"));
    EXPECT_EQ("", GetDomainFromURL("file:///C:/file.html"));
    EXPECT_EQ("", GetDomainFromURL("http://foo.com.."));
    EXPECT_EQ("", GetDomainFromURL("http://..."));
    EXPECT_EQ("", GetDomainFromURL("http://192.168.0.1"));
    EXPECT_EQ("", GetDomainFromURL("http://localhost"));
    EXPECT_EQ("", GetDomainFromURL("http://localhost."));
    EXPECT_EQ("", GetDomainFromURL("http:////Comment"));

    // Test std::string version of GetDomainAndRegistry().  Uses the same
    // underpinnings as the URI version, so this is really more of a check of
    // CanonicalizeHost().
    EXPECT_EQ("baz.jp", GetDomainFromHost("a.baz.jp"));                  // 1
    EXPECT_EQ("baz.jp.", GetDomainFromHost("a.baz.jp."));                // 1
    EXPECT_EQ("", GetDomainFromHost("ac.jp"));                           // 2
    EXPECT_EQ("", GetDomainFromHost("a.bar.jp"));                        // 3
    EXPECT_EQ("", GetDomainFromHost("bar.jp"));                          // 3
    EXPECT_EQ("", GetDomainFromHost("baz.bar.jp"));                      // 3 4
    EXPECT_EQ("a.b.baz.bar.jp", GetDomainFromHost("a.b.baz.bar.jp"));    // 3 4
    EXPECT_EQ("pref.bar.jp", GetDomainFromHost("baz.pref.bar.jp"));      // 5
    EXPECT_EQ("b.bar.baz.com.", GetDomainFromHost("a.b.bar.baz.com."));  // 6
    EXPECT_EQ("a.d.c", GetDomainFromHost("a.d.c"));                      // 7
    EXPECT_EQ("a.d.c", GetDomainFromHost(".a.d.c"));                     // 7
    EXPECT_EQ("a.d.c", GetDomainFromHost("..a.d.c"));                    // 7
    EXPECT_EQ("b.c", GetDomainFromHost("a.b.c"));                        // 7 8
    EXPECT_EQ("baz.com", GetDomainFromHost("baz.com"));                  // none
    EXPECT_EQ("baz.com.", GetDomainFromHost("baz.com."));                // none

    EXPECT_EQ("", GetDomainFromHost(std::string()));
    EXPECT_EQ("", GetDomainFromHost("foo.com.."));
    EXPECT_EQ("", GetDomainFromHost("..."));
    EXPECT_EQ("", GetDomainFromHost("192.168.0.1"));
    EXPECT_EQ("", GetDomainFromHost("localhost."));
    EXPECT_EQ("", GetDomainFromHost(".localhost."));
}

TEST_F(RegistryControlledDomainTest, TestGetRegistryLength) {
    UseDomainData(Perfect_Hash_Test1::FindDomain);

    // Test URI version of GetRegistryLength().
    EXPECT_EQ(2U, GetRegistryLengthFromURL("http://a.baz.jp/file.html",
                                           EXCLUDE_UNKNOWN_REGISTRIES)); // 1
//  EXPECT_EQ(3U, GetRegistryLengthFromURL("http://a.baz.jp./file.html",
//                                         EXCLUDE_UNKNOWN_REGISTRIES)); // 1
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://ac.jp",
                                           EXCLUDE_UNKNOWN_REGISTRIES)); // 2
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://a.bar.jp",
                                           EXCLUDE_UNKNOWN_REGISTRIES)); // 3
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://bar.jp",
                                           EXCLUDE_UNKNOWN_REGISTRIES)); // 3
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://baz.bar.jp",
                                           EXCLUDE_UNKNOWN_REGISTRIES)); // 3 4
    EXPECT_EQ(12U, GetRegistryLengthFromURL("http://a.b.baz.bar.jp",
                                            EXCLUDE_UNKNOWN_REGISTRIES)); // 4
    EXPECT_EQ(6U, GetRegistryLengthFromURL("http://baz.pref.bar.jp",
                                           EXCLUDE_UNKNOWN_REGISTRIES)); // 5
    EXPECT_EQ(11U, GetRegistryLengthFromURL("http://a.b.bar.baz.com",
                                            EXCLUDE_UNKNOWN_REGISTRIES)); // 6
    EXPECT_EQ(3U, GetRegistryLengthFromURL("http://a.d.c",
                                           EXCLUDE_UNKNOWN_REGISTRIES)); // 7
//  EXPECT_EQ(3U, GetRegistryLengthFromURL("http://.a.d.c",
//                                         EXCLUDE_UNKNOWN_REGISTRIES)); // 7
//  EXPECT_EQ(3U, GetRegistryLengthFromURL("http://..a.d.c",
//                                         EXCLUDE_UNKNOWN_REGISTRIES)); // 7
    EXPECT_EQ(1U, GetRegistryLengthFromURL("http://a.b.c",
                                           EXCLUDE_UNKNOWN_REGISTRIES)); // 7 8
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://baz.com",
                                           EXCLUDE_UNKNOWN_REGISTRIES)); // none
//  EXPECT_EQ(0U, GetRegistryLengthFromURL("http://baz.com.",
//                                         EXCLUDE_UNKNOWN_REGISTRIES)); // none
    EXPECT_EQ(3U, GetRegistryLengthFromURL("http://baz.com",
                                           INCLUDE_UNKNOWN_REGISTRIES)); // none
//  EXPECT_EQ(4U, GetRegistryLengthFromURL("http://baz.com.",
//                                         INCLUDE_UNKNOWN_REGISTRIES)); // none

    EXPECT_EQ(std::string::npos,
        GetRegistryLengthFromURL(std::string(), EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(std::string::npos,
        GetRegistryLengthFromURL("http://", EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(std::string::npos,
        GetRegistryLengthFromURL("file:///C:/file.html",
                                 EXCLUDE_UNKNOWN_REGISTRIES));
//  EXPECT_EQ(0U, GetRegistryLengthFromURL("http://foo.com..",
//                                         EXCLUDE_UNKNOWN_REGISTRIES));
//  EXPECT_EQ(0U, GetRegistryLengthFromURL("http://...",
//                                         EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://192.168.0.1",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://localhost",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://localhost",
                                           INCLUDE_UNKNOWN_REGISTRIES));
//  EXPECT_EQ(0U, GetRegistryLengthFromURL("http://localhost.",
//                                         EXCLUDE_UNKNOWN_REGISTRIES));
//  EXPECT_EQ(0U, GetRegistryLengthFromURL("http://localhost.",
//                                         INCLUDE_UNKNOWN_REGISTRIES));
//  EXPECT_EQ(0U, GetRegistryLengthFromURL("http:////Comment",
//                                         EXCLUDE_UNKNOWN_REGISTRIES));

    // Test std::string version of GetRegistryLength().  Uses the same
    // underpinnings as the URI version, so this is really more of a check of
    // CanonicalizeHost().
    EXPECT_EQ(2U, GetRegistryLengthFromHost("a.baz.jp",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // 1
    EXPECT_EQ(3U, GetRegistryLengthFromHost("a.baz.jp.",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // 1
    EXPECT_EQ(0U, GetRegistryLengthFromHost("ac.jp",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // 2
    EXPECT_EQ(0U, GetRegistryLengthFromHost("a.bar.jp",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // 3
    EXPECT_EQ(0U, GetRegistryLengthFromHost("bar.jp",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // 3
    EXPECT_EQ(0U, GetRegistryLengthFromHost("baz.bar.jp",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // 3 4
    EXPECT_EQ(12U, GetRegistryLengthFromHost("a.b.baz.bar.jp",
                                             EXCLUDE_UNKNOWN_REGISTRIES)); // 4
    EXPECT_EQ(6U, GetRegistryLengthFromHost("baz.pref.bar.jp",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // 5
    EXPECT_EQ(11U, GetRegistryLengthFromHost("a.b.bar.baz.com",
                                             EXCLUDE_UNKNOWN_REGISTRIES)); // 6
    EXPECT_EQ(3U, GetRegistryLengthFromHost("a.d.c",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // 7
    EXPECT_EQ(3U, GetRegistryLengthFromHost(".a.d.c",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // 7
    EXPECT_EQ(3U, GetRegistryLengthFromHost("..a.d.c",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // 7
    EXPECT_EQ(1U, GetRegistryLengthFromHost("a.b.c",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // 7 8
    EXPECT_EQ(0U, GetRegistryLengthFromHost("baz.com",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // none
    EXPECT_EQ(0U, GetRegistryLengthFromHost("baz.com.",
                                            EXCLUDE_UNKNOWN_REGISTRIES));  // none
    EXPECT_EQ(3U, GetRegistryLengthFromHost("baz.com",
                                            INCLUDE_UNKNOWN_REGISTRIES));  // none
    EXPECT_EQ(4U, GetRegistryLengthFromHost("baz.com.",
                                            INCLUDE_UNKNOWN_REGISTRIES));  // none

    EXPECT_EQ(std::string::npos,
        GetRegistryLengthFromHost(std::string(), EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromHost("foo.com..",
                                            EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromHost("..",
                                            EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromHost("192.168.0.1",
                                            EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromHost("localhost",
                                            EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromHost("localhost",
                                            INCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromHost("localhost.",
                                            EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromHost("localhost.",
                                            INCLUDE_UNKNOWN_REGISTRIES));
}

TEST_F(RegistryControlledDomainTest, TestSameDomainOrHost) {
    UseDomainData(Perfect_Hash_Test2::FindDomain);

    EXPECT_TRUE(CompareDomains("http://a.b.bar.jp/file.html",
                               "http://a.b.bar.jp/file.html"));  // b.bar.jp
    EXPECT_TRUE(CompareDomains("http://a.b.bar.jp/file.html",
                               "http://b.b.bar.jp/file.html"));  // b.bar.jp
    EXPECT_FALSE(CompareDomains("http://a.foo.jp/file.html",     // foo.jp
                                "http://a.not.jp/file.html"));   // not.jp
    EXPECT_FALSE(CompareDomains("http://a.foo.jp/file.html",     // foo.jp
                                "http://a.foo.jp./file.html"));  // foo.jp.
    EXPECT_FALSE(CompareDomains("http://a.com/file.html",        // a.com
                                "http://b.com/file.html"));      // b.com
    EXPECT_TRUE(CompareDomains("http://a.x.com/file.html",
                               "http://b.x.com/file.html"));     // x.com
//  EXPECT_TRUE(CompareDomains("http://a.x.com/file.html",
//                             "http://.x.com/file.html"));      // x.com
//  EXPECT_TRUE(CompareDomains("http://a.x.com/file.html",
//                             "http://..b.x.com/file.html"));   // x.com
    EXPECT_TRUE(CompareDomains("http://intranet/file.html",
                               "http://intranet/file.html"));    // intranet
    EXPECT_TRUE(CompareDomains("http://127.0.0.1/file.html",
                               "http://127.0.0.1/file.html"));   // 127.0.0.1
    EXPECT_FALSE(CompareDomains("http://192.168.0.1/file.html",  // 192.168.0.1
                                "http://127.0.0.1/file.html"));  // 127.0.0.1
    EXPECT_FALSE(CompareDomains("file:///C:/file.html",
                                "file:///C:/file.html"));        // no host
}

TEST_F(RegistryControlledDomainTest, TestDefaultData) {
    // Note that no data is set: we're using the default rules.
    EXPECT_EQ(3U, GetRegistryLengthFromURL("http://google.com",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(3U, GetRegistryLengthFromURL("http://stanford.edu",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(3U, GetRegistryLengthFromURL("http://ustreas.gov",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(3U, GetRegistryLengthFromURL("http://icann.net",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(3U, GetRegistryLengthFromURL("http://ferretcentral.org",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://nowhere.foo",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(3U, GetRegistryLengthFromURL("http://nowhere.foo",
                                           INCLUDE_UNKNOWN_REGISTRIES));
}

TEST_F(RegistryControlledDomainTest, TestPrivateRegistryHandling) {
    UseDomainData(Perfect_Hash_Test1::FindDomain);
    // Testing the same dataset for INCLUDE_PRIVATE_REGISTRIES and
    // EXCLUDE_PRIVATE_REGISTRIES arguments.
    // For the domain data used for this test, the private registries are
    // 'priv.no' and 'private'.

    // Non-private registries.
    EXPECT_EQ(2U, GetRegistryLengthFromURL("http://priv.no",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(2U, GetRegistryLengthFromURL("http://foo.priv.no",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(2U, GetRegistryLengthFromURL("http://foo.jp",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(2U, GetRegistryLengthFromURL("http://www.foo.jp",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://private",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://foo.private",
                                           EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U, GetRegistryLengthFromURL("http://private",
                                           INCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(7U, GetRegistryLengthFromURL("http://foo.private",
                                           INCLUDE_UNKNOWN_REGISTRIES));

    // Private registries.
    EXPECT_EQ(0U,
        GetRegistryLengthFromURLIncludingPrivate("http://priv.no",
                                                 EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(7U,
        GetRegistryLengthFromURLIncludingPrivate("http://foo.priv.no",
                                                 EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(2U,
        GetRegistryLengthFromURLIncludingPrivate("http://foo.jp",
                                                 EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(2U,
        GetRegistryLengthFromURLIncludingPrivate("http://www.foo.jp",
                                                 EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U,
        GetRegistryLengthFromURLIncludingPrivate("http://private",
                                                 EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(7U,
        GetRegistryLengthFromURLIncludingPrivate("http://foo.private",
                                                 EXCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(0U,
        GetRegistryLengthFromURLIncludingPrivate("http://private",
                                                 INCLUDE_UNKNOWN_REGISTRIES));
    EXPECT_EQ(7U,
        GetRegistryLengthFromURLIncludingPrivate("http://foo.private",
                                                 INCLUDE_UNKNOWN_REGISTRIES));
}


}  // namespace toft
