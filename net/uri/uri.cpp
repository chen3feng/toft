// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Description: URI class, based on RFC 3986

#include "toft/net/uri/uri.h"

#include <stddef.h>
#include <string.h>
#include <string>

#undef __DEPRECATED // disable deprecated warning
#include <strstream> // NOLINT(readability/streams)
#include <vector>

#include "toft/base/string/byte_set.h"
#include "toft/base/string/compare.h"
#include "toft/encoding/ascii.h"
#include "toft/encoding/percent.h"

namespace toft {

void UriAuthority::Swap(UriAuthority* other)
{
    using std::swap;
    swap(m_has_user_info, other->m_has_user_info);
    swap(m_has_port, other->m_has_port);
    swap(m_user_info, other->m_user_info);
    swap(m_host, other->m_host);
    swap(m_port, other->m_port);
}

// Encode

void URI::Encode(const StringPiece& src, std::string* dest)
{
    PercentEncoding::EncodeUriTo(src, dest);
}

std::string URI::Encode(const StringPiece& src)
{
    return PercentEncoding::EncodeUri(src);
}

void URI::Encode(std::string* uri)
{
    PercentEncoding::EncodeUri(uri);
}

// EncodeComponent

void URI::EncodeComponent(const StringPiece& src, std::string* dest)
{
    PercentEncoding::EncodeUriComponentTo(src, dest);
}

std::string URI::EncodeComponent(const StringPiece& src)
{
    return PercentEncoding::EncodeUriComponent(src);
}

void URI::EncodeComponent(std::string* uri)
{
    PercentEncoding::EncodeUriComponent(uri);
}

// Decode % encoding string
bool URI::Decode(const StringPiece& src, std::string* result)
{
    return PercentEncoding::DecodeTo(src, result);
}

namespace {

// collect all bytesets into one singleton and acquire in the ctor of UriParser.
// to avoid unnecessary time and space overhead.
struct UriParserByteSets
{
    const ByteSet& alpha;
    const ByteSet& upper;
    const ByteSet& lower;
    const ByteSet& digit;
    const ByteSet& alphanum;
    const ByteSet& print;
    const ByteSet& hex;

    const ByteSet gen_delims;
    const ByteSet sub_delims;
    const ByteSet reserved;
    const ByteSet mark;
    const ByteSet unreserved;
    const ByteSet userinfo;
    const ByteSet uric_no_slash;
    const ByteSet rel_segment;
    const ByteSet scheme;
    const ByteSet reg_name;
    const ByteSet pchar;
    const ByteSet query_or_fragment;

private:
    UriParserByteSets():
        alpha(ByteSet::AlphaSet()),
        upper(ByteSet::UpperSet()),
        lower(ByteSet::LowerSet()),
        digit(ByteSet::DigitSet()),
        alphanum(ByteSet::AlphaNumSet()),
        print(ByteSet::PrintSet()),
        hex(ByteSet::HexSet()),
        gen_delims(":/?#[]@"),
        sub_delims("!$&'()*+,;="),
        reserved(gen_delims | sub_delims),
        mark("-_.!~*'()"),
        unreserved(alphanum | "-._~"),
        userinfo(unreserved | ",:&=+$,"),
        uric_no_slash(unreserved | ",?:@&=+$,"),
        rel_segment(unreserved | ",@&=+$,"),
        scheme(ByteSet::AlphaNumSet() | "+-."),
        reg_name(unreserved | "$,,:@&=+"),
        pchar(unreserved | sub_delims | ":@"),
        query_or_fragment(pchar | "/?")
    {
    }
public:
    static const UriParserByteSets& Instance()
    {
        static UriParserByteSets instance;
        return instance;
    }
};

// see RFC 2396
class UriParser
{
    friend class Result;

    // Helper class to auto rollback when parse failure
    class Result
    {
    public:
        // remember current reading pointer
        explicit Result(UriParser* parser) :
            m_parser(parser),
            m_begin(parser->m_current),
            m_result(false)
        {
        }

        ~Result()
        {
            // auto rollback if not success
            if (!m_result)
                m_parser->m_current = m_begin;
        }

        operator bool() const
        {
            return m_result;
        }
        Result& operator=(bool value)
        {
            m_result = value;
            if (!value)
                m_parser->m_current = m_begin;
            return *this;
        }
        const char* begin() const
        {
            return m_begin;
        }
        const char* end() const
        {
            return m_parser->m_current;
        }
        size_t length() const
        {
            return m_parser->m_current - m_begin;
        }
        void Reset()
        {
            m_begin = m_parser->m_current;
            m_result = false;
        }
    private:
        Result(const Result&);
        Result& operator=(const Result&);
    private:
        UriParser* m_parser;
        const char* m_begin;
        bool m_result;
    };
public:
    UriParser() :
        m_byte_sets(UriParserByteSets::Instance()),
        m_begin(NULL),
        m_end(NULL),
        m_current(NULL),
        m_result(NULL)
    {
    }

    size_t Parse(const char* uri, size_t uri_length, URI* result)
    {
        m_begin = uri;
        m_current = uri;
        m_end = uri + uri_length;
        m_result = result;
        match_URI_reference();
        return m_current - m_begin;
    }

private: // in RFC 3986 Appendix A.  Collected ABNF for URI order
    // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    bool match_URI()
    {
        Result r(this);
        r = match_scheme_and_colon() && match_hier_part();
        if (r)
        {
            {
                Result r1(this);
                r1 = match_literal('?') && match_query();
                if (r1)
                    m_result->SetQuery(r1.begin() + 1, r1.length() - 1);
            }
            {
                Result r1(this);
                r1 = match_literal('#') && match_fragment();
                if (r1)
                    m_result->SetFragment(r1.begin() + 1, r1.length() - 1);
            }
        }
        return r;
    }

    // hier-part     = "//" authority path-abempty
    //               / path-absolute
    //               / path-rootless
    //               / path-empty
    bool match_hier_part()
    {
        Result r(this);
        r = match_literal("//") && match_authority() && match_path_abempty();
        if (r)
            return true;
        return match_path_absolute() ||
               match_path_rootless() ||
               match_path_empty();
    }

    // URI-reference = URI / relative-ref
    bool match_URI_reference()
    {
        return match_URI() || match_relative_ref();
    }

    // absolute-URI  = scheme ":" hier-part [ "?" query ]
    bool match_absoluteURI()
    {
        Result r(this);
        if (match_scheme_and_colon())
        {
            r = match_hier_part();
            if (r)
            {
                Result r1(this);
                r1 = match_literal('?') && match_query();
                if (r1)
                    m_result->SetQuery(r1.begin() + 1, r1.length() - 1);
            }
        }
        return r;
    }

    // relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
    bool match_relative_ref()
    {
        Result r(this);
        r = match_relative_part();
        if (r)
        {
            Result r1(this);
            r1 = match_literal('?') && match_query();
            if (r1)
                m_result->SetQuery(r1.begin() + 1, r1.length() - 1);
            return true;
        }
        return false;
    }

    // relative-part = "//" authority path-abempty
    //               / path-absolute
    //               / path-noscheme
    //               / path-empty
    bool match_relative_part()
    {
        Result r(this);
        r = match_literal("//") && match_authority() && match_path_abempty();
        if (!r)
        {
            r = match_path_absolute() ||
                match_path_noscheme() ||
                match_path_empty();
        }
        return r;
    }

    // scheme        = alpha *( alpha | digit | "+" | "-" | "." )
    // colon         = :
    bool match_scheme_and_colon()
    {
        Result r(this);
        r = match_alpha();
        if (r)
        {
            while (match_byteset(m_byte_sets.scheme))
            {
            }
            if (match_literal(':'))
                m_result->SetScheme(r.begin(), r.length() - 1);
            else
                r = false;
        }
        return r;
    }

    bool maybe_contains_userinfo() const
    {
        for (const char* p = m_current; p < m_end; ++p)
        {
            switch (*p)
            {
            case '@':
                return true;
            case '/':
                return false;
            }
        }
        return false;
    }

    // authority     = [ userinfo "@" ] host [ ":" port ]
    bool match_authority()
    {
        if (maybe_contains_userinfo())
        {
            Result r(this);
            r = match_userinfo() && match_literal('@');
            if (r)
                m_result->SetUserInfo(r.begin(), r.length() - 1);
        }

        if (!match_host())
            return false;

        Result r(this);
        r = match_literal(':') && match_port();
        if (r)
            m_result->SetPort(r.begin() + 1, r.length() - 1);

        return true;
    }

    // userinfo      = *( unreserved | escaped | ";" | ":" | "&" | "=" | "+" | "$" | "," )
    bool match_userinfo()
    {
        while (match_byteset(m_byte_sets.userinfo) || match_pct_encoded())
        {
        }
        return true;
    }

    // host          = IP-literal / IPv4address / reg-name
    bool match_host()
    {
        const char* begin = m_current;
        if (match_reg_name() || match_IPv4address() || match_IP_literal())
        {
            m_result->SetHost(begin, m_current - begin);
            return true;
        }
        return false;
    }

    // port          = *digit
    bool match_port()
    {
        while (match_byteset(m_byte_sets.digit))
        {
        }
        return true;
    }

    // IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
    // We don't accept IPv6 address to simplify the implementation
    bool match_IP_literal()
    {
        return false;
    }

    // IPv4address   = 1*digit "." 1*digit "." 1*digit "." 1*digit
    bool match_IPv4address()
    {
        Result r(this);
        for (int i = 0; i < 3; ++i)
        {
            if (match_digit())
            {
                while (match_digit())
                {
                }
                if (!match_literal('.'))
                    return false;
            }
        }

        // last field, no following dot
        if (match_digit())
        {
            while (match_digit())
            {
            }
            r = true;
        }

        return r;
    }

    // reg-name      = *( unreserved / pct-encoded / sub-delims )
    bool match_reg_name()
    {
#if 1
        // reg-name is less strict than domain, and domain is widely used,
        // so we only allow domain as reg name
        return match_hostname();
#else
        // real reg-name
        Result r(this);
        int n = 0;
        while (match_byteset(m_byte_sets.reg_name) ||
               match_pct_encoded() ||
               match_byteset(m_byte_sets.sub_delims))
        {
            ++n;
        }
        r = n > 0;
        if (r)
            m_result->SetRegName(r.begin(), r.length());
        return r;
#endif
    }

    // path          = path-abempty    ; begins with "/" or is empty
    //               / path-absolute   ; begins with "/" but not "//"
    //               / path-noscheme   ; begins with a non-colon segment
    //               / path-rootless   ; begins with a segment
    //               / path-empty      ; zero characters
    bool match_path()
    {
        return match_path_abempty() || match_path_absolute() ||
               match_path_noscheme() || match_path_rootless() ||
               match_path_empty();
    }

    // path-abempty  = *( "/" segment )
    bool match_path_abempty()
    {
        const char* begin = m_current;
        for (;;)
        {
            Result r1(this);
            r1 = match_literal('/') && match_segment();
            if (!r1)
                break;
        }
        m_result->SetPath(begin, m_current - begin);
        return true;
    }

    // path-absolute = "/" [ segment-nz *( "/" segment ) ]
    bool match_path_absolute()
    {
        const char* begin = m_current;
        if (!match_literal('/'))
            return false;
        if (match_segment_nz())
        {
            for (;;)
            {
                Result r(this);
                r = match_literal('/') && match_segment();
                if (!r)
                    break;
            }
        }
        m_result->SetPath(begin, m_current - begin);
        return true;
    }

    // path-noscheme = segment-nz-nc *( "/" segment )
    bool match_path_noscheme()
    {
        const char* begin = m_current;
        if (!match_segment_nz_nc())
            return false;
        for (;;)
        {
            Result r(this);
            r = match_literal('/') && match_segment();
            if (!r)
                break;
        }
        m_result->SetPath(begin, m_current - begin);
        return true;
    }

    // path-rootless = segment-nz *( "/" segment )
    bool match_path_rootless()
    {
        const char* begin = m_current;
        if (!match_segment_nz())
            return false;
        for (;;)
        {
            Result r(this);
            r = match_literal('/') && match_segment();
            if (!r)
                break;
        }
        m_result->SetPath(begin, m_current - begin);
        return true;
    }

    // path-empty    = 0<pchar>
    bool match_path_empty()
    {
        m_result->SetPath("", 0);
        return true;
    }

    // segment       = *pchar
    bool match_segment()
    {
        while (match_pchar())
        {
        }
        return true;
    }

    // segment-nz    = 1*pchar
    bool match_segment_nz()
    {
        int count = 0;
        while (match_pchar())
            ++count;
        return count > 0;
    }

    // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
    //               ; non-zero-length segment without any colon ":"
    bool match_segment_nz_nc()
    {
        int count = 0;
        for (;;)
        {
            Result r(this);
            r = match_unreserved() || match_pct_encoded() ||
                match_byteset(m_byte_sets.sub_delims) || match_literal('@');
            if (r)
                ++count;
            else
                break;
        }
        return count > 0;
    }


    // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    bool match_pchar()
    {
        return match_byteset(m_byte_sets.pchar) || match_pct_encoded();
    }

    // query         = *( pchar / "/" / "?" )
    bool match_query()
    {
        return match_query_or_fragment();
    }

    // fragment      = *( pchar / "/" / "?" )
    bool match_fragment()
    {
        return match_query_or_fragment();
    }

    // escaped       = "%" hex hex |
    //                 "%u" hex hex hex hex
    bool match_pct_encoded()
    {
        size_t left = m_end - m_current;
        if (left > 2 && *m_current == '%')
        {
            if (m_byte_sets.hex(m_current[1]) && m_byte_sets.hex(m_current[2]))
            {
                m_current += 3;
                return true;
            }

            // "%uXXXX"
            if (left > 5 && m_current[1] == 'u' &&
                m_byte_sets.hex(m_current[2]) && m_byte_sets.hex(m_current[3]) &&
                m_byte_sets.hex(m_current[4]) && m_byte_sets.hex(m_current[5]))
            {
                m_current += 6;
                return true;
            }
        }
        return false;
    }

    // unreserved    = alphanum | mark
    bool match_unreserved()
    {
        return match_byteset(m_byte_sets.unreserved);
    }

    // reserved      = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ","
    bool match_reserved()
    {
        return match_byteset(m_byte_sets.reserved);
    }

private: // helpers
    // hostname      = *( domainlabel "." ) toplabel [ "." ]
    bool match_hostname()
    {
        Result r(this);
        for (;;)
        {
            Result r1(this);
            r1 = match_domainlabel() && match_literal('.');
            if (!r1)
                break;
        }
        r = match_toplabel();
        if (r)
            match_literal('.');
        return r;
    }

    // domainlabel   = alphanum | alphanum *( alphanum | "-" ) alphanum
    bool match_domainlabel()
    {
        // alphanum | alphanum *( alphanum | "-" ) alphanum
        Result r(this);
        if (match_alphanum())
        {
            Result r1(this);
            for (;;)
            {
                while (match_literal('-')) // || match_literal('_')) // allow _ in domain?
                {
                }
                if (match_alphanum())
                    r1 = true;
                else
                    break;
            }
            r = true;
        }
        return r;
    }

    // toplabel      = alpha | alpha *( alphanum | "-" ) alphanum
    bool match_toplabel()
    {
        Result r(this);
        if (match_alpha())
        {
            Result r1(this);
            for (;;)
            {
                while (match_literal('-'))
                {
                }
                if (match_alphanum())
                    r1 = true;
                else
                    break;
            }
            r = true;
        }
        return r;
    }

    // they are same in RFC 3986
    bool match_query_or_fragment()
    {
        while (match_byteset(m_byte_sets.query_or_fragment) || match_pct_encoded())
        {
        }
        return true;
    }

    // mark          = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"
    bool match_mark()
    {
        return match_byteset(m_byte_sets.mark);
    }

    bool match_hex()
    {
        return match_byteset(m_byte_sets.hex);
    }

    // escaped       = "%" hex hex
    bool match_percent_encoded()
    {
        size_t left = m_end - m_current;
        if (left > 2 && m_current[0] == '%' &&
            m_byte_sets.hex(m_current[1]) && m_byte_sets.hex(m_current[2]))
        {
            m_current += 3;
            return true;
        }
        return false;
    }

    bool match_alphanum()
    {
        return match_byteset(m_byte_sets.alphanum);
    }

    bool match_alpha()
    {
        return match_byteset(m_byte_sets.alpha);
    }

    bool match_lowalpha()
    {
        return match_byteset(m_byte_sets.lower);
    }

    bool match_upalpha()
    {
        return match_byteset(m_byte_sets.upper);
    }

    bool match_digit()
    {
        return match_byteset(m_byte_sets.digit);
    }

    bool match_literal(char c)
    {
        if (m_current < m_end && *m_current == c)
        {
            ++m_current;
            return true;
        }
        return false;
    }

    template <size_t N>
    bool match_literal(const char (&l)[N])
    {
        const ptrdiff_t w = N - 1;
        if (m_end - m_current >= w && memeql(m_current, l, w))
        {
            m_current += w;
            return true;
        }
        return false;
    }

    template <typename Pred>
    bool match_byteset(const Pred& cs)
    {
        if (m_current < m_end && cs(*m_current))
        {
            ++m_current;
            return true;
        }
        return false;
    }

private:
    const UriParserByteSets& m_byte_sets;
    const char* m_begin;
    const char* m_end;
    const char* m_current;
    URI* m_result;
};

} // anonymous namespace

size_t URI::ParseBuffer(const char* uri, size_t uri_length)
{
    UriParser p;
    Clear();
    return p.Parse(uri, uri_length, this);
}


bool URI::Parse(const char* uri)
{
    size_t length = strlen(uri);
    size_t parsed_length = ParseBuffer(uri, length);
    return parsed_length == length;
}

void URI::Clear()
{
    m_scheme.clear();
    ClearAuthority();
    m_path.clear();
    ClearQuery();
    ClearFragment();
}

std::string& URI::ToString(std::string* result) const
{
    result->clear();

    if (!this->Scheme().empty())
    {
        *result += this->Scheme();
        *result += ':';
    }

    if (this->HasAuthority())
    {
        result->append("//", 2);
        if (this->HasUserInfo())
        {
            *result += this->UserInfo();
            *result += '@';
        }
        *result += this->Host();
        if (this->HasPort())
        {
            *result += ':';
            *result += Port();
        }
    }

    *result += this->Path();
    if (this->HasQuery())
    {
        *result += '?';
        *result += this->Query();
    }
    if (this->HasFragment())
    {
        *result += '#';
        *result += this->Fragment();
    }

    return *result;
}

std::string URI::ToString() const
{
    std::string s;
    ToString(&s);
    return s;
}

bool URI::WriteToBuffer(char* buffer, size_t buffer_size, size_t* result_size) const
{
    if (buffer_size < 1)
    {
        result_size = 0;
        return -1;
    }

    std::ostrstream oss(buffer, buffer_size);
    if (!this->Scheme().empty())
    {
        oss << Scheme();
        oss << ':';
    }

    if (this->HasAuthority())
    {
        oss << "//";
        if (this->HasUserInfo())
        {
            oss << this->UserInfo();
            oss << '@';
        }
        oss << this->Host();
        if (this->HasPort())
        {
            oss << ':';
            oss << this->Port();
        }
    }

    oss << this->Path();
    if (this->HasQuery())
    {
        oss << '?';
        oss << this->Query();
    }

    if (this->HasFragment())
    {
        oss << '#';
        oss << this->Fragment();
    }

    oss.flush();

    *result_size = static_cast<size_t>(oss.tellp());
    buffer[*result_size] = '\0';

    return oss.good();
}

void URI::Swap(URI* other)
{
    using namespace std;
    swap(m_has_authority, other->m_has_authority);
    swap(m_has_query, other->m_has_query);
    swap(m_has_fragment, other->m_has_fragment);
    swap(m_scheme, other->m_scheme);
    m_authority.Swap(&other->m_authority);
    swap(m_path, other->m_path);
    swap(m_query, other->m_query);
    swap(m_fragment, other->m_fragment);
}

bool URI::Normalize()
{
    StringLower(&m_scheme);
    StringLower(&m_authority.m_host);
    return true;
}

bool URI::ToAbsolute(const URI& base)
{
    return Merge(base);
}

//////////////////////////////////////////////////////////////////////////////
// merge helpers
// these lower cased function names and algorithms are taken from RFC 3986

static void remove_last_segment_and_preceding_slash(std::string* path)
{
    size_t pos = path->find_last_of('/');
    if (pos != path->npos)
    {
        path->erase(pos);
    }
}

// 5.2.4.  Remove Dot Segments
static void remove_dot_segments(const std::string& path, std::string* result)
{
    result->clear();

    std::vector<char> input(path.begin(), path.end());
    char* input_buffer = &input[0];
    size_t input_length = input.size();

    while (input_length > 0)
    {
        // rule A
        if (input_length >= 3 && memeql(input_buffer, "../", 3))
        {
            input_buffer += 3;
            input_length -= 3;
        }
        else if (input_length >= 2 && memeql(input_buffer, "./", 2))
        {
            input_buffer += 2;
            input_length -= 2;
        }
        // rule B
        else if (input_length >= 3 && memeql(input_buffer, "/./", 3))
        {
            input_buffer += 2;
            input_length -= 2;
            *input_buffer = '/';
        }
        else if (input_length == 2 && memeql(input_buffer, "/.", 2))
        {
            input_buffer += 1;
            input_length -= 1;
            *input_buffer = '/';
        }
        // rule C
        else if (input_length >= 4 && memeql(input_buffer, "/../", 4))
        {
            input_buffer += 3;
            input_length -= 3;
            *input_buffer = '/';
            remove_last_segment_and_preceding_slash(result);
        }
        else if (input_length == 3 && memeql(input_buffer, "/..", 3))
        {
            input_buffer += 2;
            input_length -= 2;
            *input_buffer = '/';
            remove_last_segment_and_preceding_slash(result);
        }
        // rule D
        else if (input_length == 1 && *input_buffer == '.')
        {
            ++input_buffer;
            --input_length;
        }
        else if (input_length == 2 && memeql(input_buffer, "..", 2))
        {
            input_buffer += 2;
            input_length -= 2;
        }
        // rule E
        else
        {
            char* p = reinterpret_cast<char*>(memchr(input_buffer + 1, '/', input_length - 1));
            if (p)
            {
                result->append(input_buffer, p - input_buffer);
                input_length -= p - input_buffer;
                input_buffer = p;
            }
            else
            {
                result->append(input_buffer, input_length);
                input_length = 0;
            }
        }
    }
}

static std::string remove_dot_segments(const std::string& path)
{
    std::string result;
    remove_dot_segments(path, &result);
    return result;
}

static void merge_path(
    bool base_has_authority,
    const std::string& base_path,
    const std::string& path,
    std::string* result
    )
{
    if (base_has_authority && base_path.empty())
    {
        *result = '/';
        *result += path;
    }
    else
    {
        size_t pos = base_path.find_last_of('/');
        if (pos != result->npos)
        {
            result->assign(base_path, 0, pos + 1);
            *result += path;
        }
        else
        {
            *result = path;
        }
    }
}

static std::string merge_path(
    bool base_has_authority,
    const std::string& base_path,
    const std::string& path)
{
    std::string result;
    merge_path(base_has_authority, base_path, path, &result);
    return result;
}


// RFC 3986
// 5.2.2.  Transform References
bool URI::Merge(const URI& base, bool strict)
{
    URI result;

    bool scheme_undefined = (!strict && this->Scheme() == base.Scheme());

    if (!scheme_undefined && !this->Scheme().empty())
    {
        result.SetScheme(this->Scheme());
        if (this->HasAuthority())
            result.SetAuthority(this->Authority());
        result.SetPath(remove_dot_segments(this->Path()));
        if (this->HasQuery())
            result.SetQuery(this->Query());
    }
    else
    {
        if (this->HasAuthority())
        {
            if (this->HasAuthority())
                result.SetAuthority(this->Authority());
            result.SetPath(remove_dot_segments(this->Path()));
            if (this->HasQuery())
                result.SetQuery(this->Query());
        }
        else
        {
            if (this->Path().empty())
            {
                result.SetPath(base.Path());
                if (this->HasQuery())
                    result.SetQuery(this->Query());
                else if (base.HasQuery())
                    result.SetQuery(base.Query());
            }
            else
            {
                std::string path;
                if (!this->Path().empty() && this->Path()[0] == '/')
                {
                    path = remove_dot_segments(this->Path());
                }
                else
                {
                    path = merge_path(base.HasAuthority(), base.Path(), this->Path());
                    path = remove_dot_segments(path);
                }
                result.SetPath(path);
                if (this->HasQuery())
                    result.SetQuery(this->Query());
            }
            if (base.HasAuthority())
                result.SetAuthority(base.Authority());
        }
        result.SetScheme(base.Scheme());
    }

    if (this->HasFragment())
    {
        result.SetFragment(this->Fragment());
    }

    this->Swap(&result);
    return true;
}

} // namespace toft

