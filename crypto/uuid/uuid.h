// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>
//  Port code from chromium/src/third_party/WebKit/Source/core/platform/UUID.h

#ifndef TOFT_CRYPTO_UUID_UUID_H
#define TOFT_CRYPTO_UUID_UUID_H

#include <string>

namespace toft {

// Creates a UUID that consists of 32 hexadecimal digits and returns its canonical form.
// The canonical form is displayed in 5 groups separated by hyphens,
// in the form 8-4-4-4-12 for a total of 36 characters.
// The hexadecimal values "a" through "f" are output as lower case characters.
//
// Note: for security reason, we should always generate version 4 UUID that
// use a scheme relying only on random numbers.
// This algorithm sets the version number as well as two reserved bits.
// All other bits are set using a random or pseudorandom
// data source. Version 4 UUIDs have the form xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
//  with hexadecimal digits for x and one of 8,9, A, or B for y.
//
// On Windows, version 4 UUIDs are used since Windows 2000 (http://msdn.microsoft.com/en-us/library/aa446557.aspx).
// On MacOSX, version 4 UUIDs are used since Tiger (http://developer.apple.com/mac/library/technotes/tn/tn1103.html#TNTAG8).
// On Linux, the kernel offers the procfs pseudo-file /proc/sys/kernel/random/uuid that
// yields version 4 UUIDs (http://hbfs.wordpress.com/2008/09/30/ueid-unique-enough-ids/).
// NOTE: Now we support linux system only
std::string CreateCanonicalUUIDString();

}  // namespace toft

#endif  // TOFT_CRYPTO_UUID_UUID_H
