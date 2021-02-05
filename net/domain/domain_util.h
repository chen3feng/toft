    // Copyright (c) 2012 The Chromium Authors. All rights reserved.
    // Use of this source code is governed by a BSD-style license that can be
    // found in the LICENSE file.

    // NB: Modelled after Mozilla's code (originally written by Pamela Greene,
    // later modified by others), but almost entirely rewritten for Chrome.
    //   (netwerk/dns/src/nsEffectiveTLDService.h)
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla TLD Service
 *
 * The Initial Developer of the Original Code is
 * Google Inc.
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Pamela Greene <pamg.bugs@gmail.com> (original author)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/*
 (Documentation based on the Mozilla documentation currently at
 http://wiki.mozilla.org/Gecko:Effective_TLD_Service, written by the same
 author.)

 The RegistryControlledDomainService examines the hostname of a URI passed to
 it and determines the longest portion that is controlled by a registrar.
 Although technically the top-level domain (TLD) for a hostname is the last
 dot-portion of the name (such as .com or .org), many domains (such as co.uk)
 function as though they were TLDs, allocating any number of more specific,
 essentially unrelated names beneath them.  For example, .uk is a TLD, but
 nobody is allowed to register a domain directly under .uk; the "effective"
 TLDs are ac.uk, co.uk, and so on.  We wouldn't want to allow any site in
 *.co.uk to set a cookie for the entire co.uk domain, so it's important to be
 able to identify which higher-level domains function as effective TLDs and
 which can be registered.

 The service obtains its information about effective TLDs from a text resource
 that must be in the following format:

 * It should use plain ASCII.
 * It should contain one domain rule per line, terminated with \n, with nothing
 else on the line.  (The last rule in the file may omit the ending \n.)
 * Rules should have been normalized using the same canonicalization that URI
 applies.  For ASCII, that means they're not case-sensitive, among other
 things; other normalizations are applied for other characters.
 * Each rule should list the entire TLD-like domain name, with any subdomain
 portions separated by dots (.) as usual.
 * Rules should neither begin nor end with a dot.
 * If a hostname matches more than one rule, the most specific rule (that is,
 the one with more dot-levels) will be used.
 * Other than in the case of wildcards (see below), rules do not implicitly
 include their subcomponents.  For example, "bar.baz.uk" does not imply
 "baz.uk", and if "bar.baz.uk" is the only rule in the list, "foo.bar.baz.uk"
 will match, but "baz.uk" and "qux.baz.uk" won't.
 * The wildcard character '*' will match any valid sequence of characters.
 * Wildcards may only appear as the entire most specific level of a rule.  That
 is, a wildcard must come at the beginning of a line and must be followed by
 a dot.  (You may not use a wildcard as the entire rule.)
 * A wildcard rule implies a rule for the entire non-wildcard portion.  For
 example, the rule "*.foo.bar" implies the rule "foo.bar" (but not the rule
 "bar").  This is typically important in the case of exceptions (see below).
 * The exception character '!' before a rule marks an exception to a wildcard
 rule.  If your rules are "*.tokyo.jp" and "!pref.tokyo.jp", then
 "a.b.tokyo.jp" has an effective TLD of "b.tokyo.jp", but "a.pref.tokyo.jp"
 has an effective TLD of "tokyo.jp" (the exception prevents the wildcard
 match, and we thus fall through to matching on the implied "tokyo.jp" rule
 from the wildcard).
 * If you use an exception rule without a corresponding wildcard rule, the
 behavior is undefined.

 Firefox has a very similar service, and it's their data file we use to
 construct our resource.  However, the data expected by this implementation
 differs from the Mozilla file in several important ways:
 (1) We require that all single-level TLDs (com, edu, etc.) be explicitly
 listed.  As of this writing, Mozilla's file includes the single-level
 TLDs too, but that might change.
 (2) Our data is expected be in pure ASCII: all UTF-8 or otherwise encoded
 items must already have been normalized.
 (3) We do not allow comments, rule notes, blank lines, or line endings other
 than LF.
 Rules are also expected to be syntactically valid.

 The utility application tld_cleanup.exe converts a Mozilla-style file into a
 Chrome one, making sure that single-level TLDs are explicitly listed, using
 URI to normalize rules, and validating the rules.
 */

#ifndef TOFT_NET_DOMAIN_DOMAIN_UTIL_H
#define TOFT_NET_DOMAIN_DOMAIN_UTIL_H

#include <string.h>
#include <string>

#include "toft/base/static_class.h"

struct DomainRule;

namespace toft {
class URI;

// This enum is a required parameter to all public methods declared for this
// service. The Public Suffix List (http://publicsuffix.org/) this service
// uses as a data source splits all effective-TLDs into two groups. The main
// group describes registries that are acknowledged by ICANN. The second group
// contains a list of private additions for domains that enable external users
// to create subdomains, such as appspot.com.
// The RegistryFilter enum lets you choose whether you want to include the
// private additions in your lookup.
// See this for example use cases:
// https://wiki.mozilla.org/Public_Suffix_List/Use_Cases
enum PrivateRegistryFilter {
    EXCLUDE_PRIVATE_REGISTRIES = 0,
    INCLUDE_PRIVATE_REGISTRIES
};

// This enum is a required parameter to the GetRegistryLength functions
// declared for this service. Whenever there is no matching rule in the
// effective-TLD data (or in the default data, if the resource failed to
// load), the result will be dependent on which enum value was passed in.
// If EXCLUDE_UNKNOWN_REGISTRIES was passed in, the resulting registry length
// will be 0. If INCLUDE_UNKNOWN_REGISTRIES was passed in, the resulting
// registry length will be the length of the last subcomponent (eg. 3 for
// foobar.baz).
enum UnknownRegistryFilter {
    EXCLUDE_UNKNOWN_REGISTRIES = 0,
    INCLUDE_UNKNOWN_REGISTRIES
};

struct DomainUtil {
    TOFT_DECLARE_STATIC_CLASS(DomainUtil);

public:
    // Returns the registered, organization-identifying host and all its registry
    // information, but no subdomains, from the given URI.  Returns an empty
    // string if the URI is invalid, has no host (e.g. a file: URL), has multiple
    // trailing dots, is an IP address, has only one subcomponent (i.e. no dots
    // other than leading/trailing ones), or is itself a recognized registry
    // identifier.  If no matching rule is found in the effective-TLD data (or in
    // the default data, if the resource failed to load), the last subcomponent of
    // the host is assumed to be the registry.
    //
    // Examples:
    //   http://www.google.com/file.html -> "google.com"  (com)
    //   http://..google.com/file.html   -> "google.com"  (com)
    //   http://google.com./file.html    -> "google.com." (com)
    //   http://a.b.co.uk/file.html      -> "b.co.uk"     (co.uk)
    //   file:///C:/bar.html             -> ""            (no host)
    //   http://foo.com../file.html      -> ""            (multiple trailing dots)
    //   http://192.168.0.1/file.html    -> ""            (IP address)
    //   http://bar/file.html            -> ""            (no subcomponents)
    //   http://co.uk/file.html          -> ""            (host is a registry)
    //   http://foo.bar/file.html        -> "foo.bar"     (no rule; assume bar)
    static std::string GetDomainAndRegistry(const toft::URI& uri, PrivateRegistryFilter filter);

    // Like the URI version, but takes a host (which is canonicalized internally)
    // instead of a full URI.
    static std::string GetDomainAndRegistry(const std::string& host, PrivateRegistryFilter filter);

    // This convenience function returns true if the two GURLs both have hosts
    // and one of the following is true:
    // * They each have a known domain and registry, and it is the same for both
    //   URLs.  Note that this means the trailing dot, if any, must match too.
    // * They don't have known domains/registries, but the hosts are identical.
    // Effectively, callers can use this function to check whether the input URLs
    // represent hosts "on the same site".
    static bool SameDomainOrHost(const toft::URI& gurl1, const toft::URI& gurl2,
                                 PrivateRegistryFilter filter);

    // Finds the length in bytes of the registrar portion of the host in the
    // given URI.  Returns std::string::npos if the URI is invalid or has no
    // host (e.g. a file: URL).  Returns 0 if the URI has multiple trailing dots,
    // is an IP address, has no subcomponents, or is itself a recognized registry
    // identifier.  The result is also dependent on the UnknownRegistryFilter.
    // If no matching rule is found in the effective-TLD data (or in
    // the default data, if the resource failed to load), returns 0 if
    // |unknown_filter| is EXCLUDE_UNKNOWN_REGISTRIES, or the length of the last
    // subcomponent if |unknown_filter| is INCLUDE_UNKNOWN_REGISTRIES.
    //
    // Examples:
    //   http://www.google.com/file.html -> 3                 (com)
    //   http://..google.com/file.html   -> 3                 (com)
    //   http://google.com./file.html    -> 4                 (com)
    //   http://a.b.co.uk/file.html      -> 5                 (co.uk)
    //   file:///C:/bar.html             -> std::string::npos (no host)
    //   http://foo.com../file.html      -> 0                 (multiple trailing
    //                                                         dots)
    //   http://192.168.0.1/file.html    -> 0                 (IP address)
    //   http://bar/file.html            -> 0                 (no subcomponents)
    //   http://co.uk/file.html          -> 0                 (host is a registry)
    //   http://foo.bar/file.html        -> 0 or 3, depending (no rule; assume
    //                                                         bar)
    static size_t GetRegistryLength(const toft::URI& uri, UnknownRegistryFilter unknown_filter,
                                    PrivateRegistryFilter private_filter);

    // Like the URI version, but takes a host (which is canonicalized internally)
    // instead of a full URI.
    static size_t GetRegistryLength(const std::string& host, UnknownRegistryFilter unknown_filter,
                                    PrivateRegistryFilter private_filter);
};

typedef const struct DomainRule* (*FindDomainPtr)(const char *, unsigned int);
// Used for unit tests, so that a different perfect hash map from the full
// list is used. Set to NULL to use the Default function.
void SetFindDomainFunctionForTesting(FindDomainPtr fn);

}  // namespace toft

#endif  // TOFT_NET_DOMAIN_DOMAIN_UTIL_H
