// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//  NOTE: Port code from from :
//  http://src.chromium.org/viewvc/chrome/trunk/src/base/hash.h

#ifndef TOFT_HASH_SUPER_FAST_H_
#define TOFT_HASH_SUPER_FAST_H_

#include <stdint.h>

#include <string>

namespace toft {

// From http://www.azillionmonkeys.com/qed/hash.html
// This is the hash used on WebCore/platform/stringhash
uint32_t SuperFastHash(const char * data, int len);

inline uint32_t SuperFastHash(const std::string& key) {
    if (key.empty()) {
        return 0;
    }
    return SuperFastHash(key.data(), static_cast<int>(key.size()));
}
}  // namespace toft

#endif  // TOFT_HASH_SUPER_FAST_H_
