// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// NB: Modelled after Mozilla's code (originally written by Pamela Greene,
// later modified by others), but almost entirely rewritten for Chrome.
//   (netwerk/dns/src/nsEffectiveTLDService.cpp)
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
 * The Original Code is Mozilla Effective-TLD Service
 *
 * The Initial Developer of the Original Code is
 * Google Inc.
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Pamela Greene <pamg.bugs@gmail.com> (original author)
 *   Daniel Witte <dwitte@stanford.edu>
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

#include "toft/net/domain/registry_controlled_domain.h"

#include "toft/net/domain/effective_tld_names.cc"
#include "toft/net/uri/uri.h"

#include "thirdparty/glog/logging.h"

#define NOTREACHED() DCHECK(false)

namespace net {
namespace registry_controlled_domains {

namespace {

const int kExceptionRule = 1;
const int kWildcardRule = 2;

const FindDomainPtr kDefaultFindDomainFunction = Perfect_Hash::FindDomain;
FindDomainPtr g_find_domain_function = kDefaultFindDomainFunction;

size_t GetRegistryLengthImpl(const std::string& host, UnknownRegistryFilter unknown_filter,
                             PrivateRegistryFilter private_filter) {
    DCHECK(!host.empty());

    // Skip leading dots.
    const size_t host_check_begin = host.find_first_not_of('.');
    if (host_check_begin == std::string::npos)
        return 0;  // Host is only dots.

    // A single trailing dot isn't relevant in this determination, but does need
    // to be included in the final returned length.
    size_t host_check_len = host.length();
    if (host[host_check_len - 1] == '.') {
        --host_check_len;
        DCHECK(host_check_len > 0);  // NOLINT
        // If this weren't true, the host would be ".",
        // and we'd have already returned above.
        if (host[host_check_len - 1] == '.')
            return 0;  // Multiple trailing dots.
    }

    // Walk up the domain tree, most specific to least specific,
    // looking for matches at each level.
    size_t prev_start = std::string::npos;
    size_t curr_start = host_check_begin;
    size_t next_dot = host.find('.', curr_start);
    if (next_dot >= host_check_len)  // Catches std::string::npos as well.
        return 0;  // This can't have a registry + domain.
    while (1) {
        const char* domain_str = host.data() + curr_start;
        int domain_length = host_check_len - curr_start;
        const DomainRule* rule = g_find_domain_function(domain_str, domain_length);

        // We need to compare the string after finding a match because the
        // no-collisions of perfect hashing only refers to items in the set.  Since
        // we're searching for arbitrary domains, there could be collisions.
        // Furthermore, if the apparent match is a private registry and we're not
        // including those, it can't be an actual match.
        if (rule && (private_filter == INCLUDE_PRIVATE_REGISTRIES || !rule->is_private)
                        && strncasecmp(domain_str, rule->name, domain_length) == 0) {
            // Exception rules override wildcard rules when the domain is an exact
            // match, but wildcards take precedence when there's a subdomain.
            if (rule->type == kWildcardRule && (prev_start != std::string::npos)) {
                // If prev_start == host_check_begin, then the host is the registry
                // itself, so return 0.
                return (prev_start == host_check_begin) ? 0 : (host.length() - prev_start);
            }

            if (rule->type == kExceptionRule) {
                if (next_dot == std::string::npos) {
                    // If we get here, we had an exception rule with no dots (e.g.
                    // "!foo").  This would only be valid if we had a corresponding
                    // wildcard rule, which would have to be "*".  But we explicitly
                    // disallow that case, so this kind of rule is invalid.
                    NOTREACHED() << "Invalid exception rule";
                    return 0;
                }
                return host.length() - next_dot - 1;
            }

            // If curr_start == host_check_begin, then the host is the registry
            // itself, so return 0.
            return (curr_start == host_check_begin) ? 0 : (host.length() - curr_start);
        }

        if (next_dot >= host_check_len)  // Catches std::string::npos as well.
            break;

        prev_start = curr_start;
        curr_start = next_dot + 1;
        next_dot = host.find('.', curr_start);
    }

    // No rule found in the registry.  curr_start now points to the first
    // character of the last subcomponent of the host, so if we allow unknown
    // registries, return the length of this subcomponent.
    return unknown_filter == INCLUDE_UNKNOWN_REGISTRIES ? (host.length() - curr_start) : 0;
}

std::string GetDomainAndRegistryImpl(const std::string& host,
                                     PrivateRegistryFilter private_filter) {
    DCHECK(!host.empty());

    // Find the length of the registry for this host.
    const size_t registry_length = GetRegistryLengthImpl(host, INCLUDE_UNKNOWN_REGISTRIES,
                                                         private_filter);
    if ((registry_length == std::string::npos) || (registry_length == 0))
        return std::string();  // No registry.
    // The "2" in this next line is 1 for the dot, plus a 1-char minimum preceding
    // subcomponent length.
    DCHECK(host.length() >= 2);  // NOLINT
    if (registry_length > (host.length() - 2)) {
        NOTREACHED() << "Host does not have at least one subcomponent before registry!";
        return std::string();
    }

    // Move past the dot preceding the registry, and search for the next previous
    // dot.  Return the host from after that dot, or the whole host when there is
    // no dot.
    const size_t dot = host.rfind('.', host.length() - registry_length - 2);
    if (dot == std::string::npos)
        return host;
    return host.substr(dot + 1);
}

}  // namespace

static bool HostIsIPAddress(const std::string& host) {
    // FIXME(yeshunping) : support ipv6
    int dot_num = 0;
    for (size_t i = 0; i < host.length(); ++i) {
        if (host[i] != '.' && !isdigit(host[i])) {
            return false;
        } else if (host[i] == '.') {
            ++dot_num;
        }
    }
    return dot_num == 3;
}

std::string GetDomainAndRegistry(const toft::URI& uri, PrivateRegistryFilter filter) {
    const std::string& host = uri.Host();
    if ((host.length() <= 0) || HostIsIPAddress(host))
        return std::string();
    return GetDomainAndRegistryImpl(host, filter);
}

std::string GetDomainAndRegistry(const std::string& host, PrivateRegistryFilter filter) {
    if (host.empty() || HostIsIPAddress(host))
        return std::string();
    return GetDomainAndRegistryImpl(host, filter);
}

bool SameDomainOrHost(const toft::URI& gurl1, const toft::URI& gurl2,
                      PrivateRegistryFilter filter) {
    // See if both URLs have a known domain + registry, and those values are the
    // same.
    const std::string domain1(GetDomainAndRegistry(gurl1, filter));
    const std::string domain2(GetDomainAndRegistry(gurl2, filter));
    if (!domain1.empty() || !domain2.empty())
        return domain1 == domain2;

    // No domains.  See if the hosts are identical.
    const std::string& host1 = gurl1.Host();
    const std::string& host2 = gurl2.Host();
    if ((host1.length() <= 0) || (host1.length() != host2.length()))
        return false;
    return host1 == host2;
}

size_t GetRegistryLength(const toft::URI& uri, UnknownRegistryFilter unknown_filter,
                         PrivateRegistryFilter private_filter) {
    const std::string& host = uri.Host();
    if (host.length() <= 0)
        return std::string::npos;
    if (HostIsIPAddress(host))
        return 0;
    return GetRegistryLengthImpl(host, unknown_filter, private_filter);
}

size_t GetRegistryLength(const std::string& host, UnknownRegistryFilter unknown_filter,
                         PrivateRegistryFilter private_filter) {
    if (host.empty())
        return std::string::npos;
    if (HostIsIPAddress(host))
        return 0;
    return GetRegistryLengthImpl(host, unknown_filter, private_filter);
}

void SetFindDomainFunctionForTesting(FindDomainPtr function) {
    g_find_domain_function = function ? function : kDefaultFindDomainFunction;
}

}  // namespace registry_controlled_domains
}  // namespace net
