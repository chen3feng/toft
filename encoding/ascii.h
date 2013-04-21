// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_ENCODING_ASCII_H
#define TOFT_ENCODING_ASCII_H

#include <limits.h>
#include <stdint.h>

namespace toft {

// 专为 ASCII 特定的函数，如果涉及到 locale，请使用标准 C 的 ctype.h
// 里定义的函数。
struct Ascii
{
private:
    Ascii();
    ~Ascii();
private:
    /// 字符类型的掩码
    enum CharTypeMask
    {
        kUpper = 1 << 0,
        kLower = 1 << 1,
        kDigit = 1 << 2,
        kHexDigit = 1 << 3,
        kBlank = 1 << 4,
        kSpace = 1 << 5,
        kControl = 1 << 6,
        kPunct = 1 << 7,
        kPrint = 1 << 8,
        kGraph = 1 << 9,
    };
public:
    /** 判断是不是有效的 ASCII 码 */
    static bool IsValid(char c)
    {
        return (c & 0x80) == 0;
    }

    static inline bool IsLower(char c)
    {
        return CharIncludeAnyTypeMask(c, kLower);
    }

    static inline bool IsUpper(char c)
    {
        return CharIncludeAnyTypeMask(c, kUpper);
    }

    /** 判断是否为字母 */
    static bool IsAlpha(char c)
    {
        return CharIncludeAnyTypeMask(c, kUpper | kLower);
    }

    /** 判断是否为数字 */
    static bool IsDigit(char c)
    {
        return CharIncludeAnyTypeMask(c, kDigit);
    }

    /** 判断是否为英文或数字  */
    static bool IsAlphaNumber(char c)
    {
        return CharIncludeAnyTypeMask(c, kUpper | kLower | kDigit);
    }

    /** 判断是否为空白字符。空格,'\t', ' ' 算作空白字符*/
    static bool IsBlank(char c)
    {
        return CharIncludeAnyTypeMask(c, kBlank);
    }

    /** 判断是否为间隔字符。*/
    static inline bool IsSpace(char c)
    {
        return CharIncludeAnyTypeMask(c, kSpace);
    }

    /** 判断是否为控制字符。*/
    static bool IsControl(char c)
    {
        return CharIncludeAnyTypeMask(c, kControl);
    }

    /** 判断是否为标点符号字符。*/
    static inline bool IsPunct(char c)
    {
        return CharIncludeAnyTypeMask(c, kPunct);
    }

    /** 判断是否为十六进制数字字符。*/
    static inline bool IsHexDigit(char c)
    {
        return CharIncludeAnyTypeMask(c, kHexDigit);
    }

    /** 判断是否为可见字符。*/
    static inline bool IsGraph(char c)
    {
        return CharIncludeAnyTypeMask(c, kGraph);
    }

    /** 判断是否为可打印字符。*/
    static inline bool IsPrint(char c)
    {
        return CharIncludeAnyTypeMask(c, kPrint);
    }

    static inline char ToAscii(char c)
    {
        return c & 0x7F;
    }

    static inline char ToLower(char c)
    {
        return IsUpper(c) ? c + ('a' - 'A') : c;
    }

    static inline char ToUpper(char c)
    {
        return IsLower(c) ? c - ('a' - 'A') : c;
    }

private:
    static int GetCharTypeMask(char c)
    {
        return table[static_cast<unsigned char>(c)];
    }

    static bool CharIncludeAnyTypeMask(char c, int mask)
    {
        return (GetCharTypeMask(c) & mask) != 0;
    }

    static bool CharIncludeAallTypeMask(char c, int mask)
    {
        return (GetCharTypeMask(c) & mask) == mask;
    }

private:
    static const uint16_t table[UCHAR_MAX + 1];
};

} // namespace toft

#endif // TOFT_ENCODING_ASCII_H
