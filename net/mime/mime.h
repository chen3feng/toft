// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_NET_MIME_MIME_H
#define TOFT_NET_MIME_MIME_H
#pragma once

#include <map>
#include <stdexcept>
#include <string>

namespace toft {

class MimeType
{
public:
    MimeType() {}

    MimeType(const std::string& type, const std::string& subtype)
        :m_type(type), m_subtype(subtype) {}

    explicit MimeType(const std::string& mime)
    {
        if (!Set(mime))
            throw std::runtime_error("Invalid MIME: " + mime);
    }

    void Set(const std::string& type, const std::string& subtype)
    {
        m_type = type;
        m_subtype = subtype;
    }

    bool Set(const std::string& mime);

    const std::string& Type() const
    {
        return m_type;
    }

    const std::string& SubType() const
    {
        return m_subtype;
    }

    bool Match(const MimeType& mime) const;

    bool Match(const std::string& mime) const;

    bool Empty()
    {
        return (m_type.empty() && m_subtype.empty());
    }
    /// convert mime to string
    std::string ToString() const
    {
        return m_type + "/" + m_subtype;
    }

    // .xml -> text/xml
    //  /etc/mime.types
    bool FromFileExtension(const std::string& ext);

private:
    typedef std::map<std::string, std::string> MapType;
    // Initialize mime map
    static MapType& GetMap();
    static MapType& DoGetMap();

    std::string m_type;
    std::string m_subtype;
};

} // namespace toft

#endif // TOFT_NET_MIME_MIME_H
