// Copyright (c) 2015, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/static_resource.h"

namespace toft {

StaticResourcePackage::StaticResourcePackage(
    const BladeResourceEntry* entry, unsigned length)
    : m_resources(entry), m_length(length) {
}

bool StaticResourcePackage::Find(StringPiece name, StringPiece* data) const {
    for (unsigned int i = 0; i < m_length; ++i) {
        if (m_resources[i].name == name) {
            data->assign(m_resources[i].data, m_resources[i].size);
            return true;
        }
    }
    return false;
}

} // namespace toft
