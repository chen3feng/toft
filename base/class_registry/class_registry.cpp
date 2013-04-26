// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-03-02

#include "toft/base/class_registry/class_registry.h"

namespace toft {

void ClassRegistryBase::DoAddClass(const std::string& entry_name,
                                   GetFunction getter) {
    ClassMap::iterator it = m_class_map.find(entry_name);
    if (it != m_class_map.end()) {
        fprintf(stderr,
                "ClassRegister: class %s already registered. "
                "One possibility: Duplicated class definition "
                "(maybe the same source file) is being linked into "
                "more than one shared libraries or both shared "
                "library and executable\n",
                entry_name.c_str());
        abort();
    }
    m_class_map[entry_name] = getter;
    m_class_names.push_back(entry_name);
}

void* ClassRegistryBase::DoGetObject(const std::string& entry_name) const {
    ClassMap::const_iterator it =
        m_class_map.find(entry_name);
    if (it == m_class_map.end()) {
        return NULL;
    }
    return (*(it->second))();
}

size_t ClassRegistryBase::ClassCount() const {
    return m_class_names.size();
}

const std::string& ClassRegistryBase::ClassName(size_t i) const {
    assert(i < m_class_names.size());
    return m_class_names[i];
}

} // namespace toft
