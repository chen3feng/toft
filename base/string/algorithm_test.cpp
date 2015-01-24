// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/string/algorithm.h"

#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "thirdparty/gtest/gtest.h"

using namespace std;

const char* kTestStringLiteral = "\\.*?wangyu";

namespace toft {

TEST(String, WhiteSpace)
{
    ASSERT_TRUE(IsWhiteString(""));
    ASSERT_TRUE(IsWhiteString("\t\r\n "));
    ASSERT_FALSE(IsWhiteString("\t\r\\"));
    ASSERT_TRUE(IsCharInString('\\', "c:\\"));
}

TEST(String, Prefix)
{
    ASSERT_TRUE(StringStartsWith("\\.*?xiaokang", "\\."));
    ASSERT_FALSE(StringStartsWith("\\.*?xiaokang", ".*"));

    ASSERT_TRUE(StringStartsWithIgnoreCase("xiaokang", "XIAOKANG"));

    ASSERT_EQ("*?xiaokang", StringRemovePrefix("\\.*?xiaokang", "\\."));
    ASSERT_EQ("\\.*?xiaokang", StringRemovePrefix("\\.*?xiaokang", ""));
    ASSERT_EQ("", StringRemovePrefix("\\.*?xiaokang", "\\.*?xiaokang"));

    string test_string = kTestStringLiteral;
    ASSERT_TRUE(StringRemovePrefix(&test_string, "\\."));
    ASSERT_EQ("*?wangyu", test_string);

    test_string = kTestStringLiteral;
    ASSERT_TRUE(StringRemovePrefix(&test_string, ""));
    ASSERT_EQ(kTestStringLiteral, test_string);

    test_string = kTestStringLiteral;
    ASSERT_TRUE(StringRemovePrefix(&test_string, kTestStringLiteral));
    ASSERT_EQ("", test_string);

    test_string = kTestStringLiteral;
    ASSERT_FALSE(StringRemovePrefix(&test_string, "phongchen"));
    ASSERT_EQ(kTestStringLiteral, test_string);
}

TEST(String, Suffix)
{
    ASSERT_TRUE(StringEndsWith("\\.*?xiaokang", ""));
    ASSERT_TRUE(StringEndsWith("\\.*?xiaokang", "kang"));
    ASSERT_EQ("\\.*?xiao", StringRemoveSuffix("\\.*?xiaokang", "kang"));
    ASSERT_EQ("\\.*?xiaokang", StringRemoveSuffix("\\.*?xiaokang", "n"));
    ASSERT_EQ("", StringRemoveSuffix("\\.*?xiaokang", "\\.*?xiaokang"));

    string test_string = kTestStringLiteral;
    ASSERT_TRUE(StringRemoveSuffix(&test_string, "yu"));
    ASSERT_EQ("\\.*?wang", test_string);

    test_string = kTestStringLiteral;
    ASSERT_FALSE(StringRemoveSuffix(&test_string, "g"));
    ASSERT_EQ(kTestStringLiteral, test_string);

    test_string = kTestStringLiteral;
    ASSERT_TRUE(StringRemoveSuffix(&test_string, kTestStringLiteral));
    ASSERT_EQ("", test_string);
}

TEST(String, StringTrim)
{
    string str = " ab end  ";
    ASSERT_EQ("ab end  ", StringTrimLeft(str));
    ASSERT_EQ(" ab end", StringTrimRight(str));
    ASSERT_EQ("ab end", StringTrim(str));

    StringTrimLeft(&str);
    ASSERT_EQ("ab end  ", str);
    StringTrimRight(&str);
    ASSERT_EQ("ab end", str);
    str = " ab end  ";
    StringTrim(&str);
    ASSERT_EQ("ab end", str);
    StringTrim(&str, "ab");
    ASSERT_EQ(" end", str);

    str = "\r\n        \t\t\t\t";
    ASSERT_EQ("", StringTrimLeft(str));
    ASSERT_EQ("", StringTrimRight(str));
    ASSERT_EQ("", StringTrim(str));

    str = "abababab";
    ASSERT_EQ("", StringTrimLeft(str, "ab"));
    ASSERT_EQ("", StringTrimRight(str, "ab"));
    ASSERT_EQ("", StringTrim(str, "ab"));
}

TEST(String, StringTrimNul)
{
    string str("\0ab end\0\0", 9);
    const string left_trimed("ab end\0\0", 8);
    const string right_trimed("\0ab end", 7);
    const string trimed = "ab end";
    EXPECT_EQ(left_trimed, StringTrimLeft(str, StringPiece("\0", 1)));
    EXPECT_EQ(right_trimed, StringTrimRight(str, StringPiece("\0", 1)));
    EXPECT_EQ(trimed, StringTrim(str, StringPiece("\0", 1)));
    EXPECT_EQ(left_trimed, StringTrimLeft(str, '\0'));
    EXPECT_EQ(right_trimed, StringTrimRight(str, '\0'));
    EXPECT_EQ(trimed, StringTrim(str, '\0'));
}

TEST(String, UpperAndLowerCase)
{
    string str = " abcdefg \\end ";
    string str_temp = str;
    StringToUpper(&str);
    ASSERT_EQ(" ABCDEFG \\END ", str);
    StringToLower(&str);
    ASSERT_EQ(str_temp, str);
    ASSERT_EQ("A中B文C测D试E", UpperString("a中b文c测d试e"));
    ASSERT_EQ("a中b文c测d试e", LowerString("A中B文C测D试E"));
}

template <typename StringType>
static void TestSplitString() {
    string str = " ab c  d   efg  end ";
    vector<string> vec;
    SplitString(str, " ", &vec);
    ASSERT_EQ(5U, vec.size());

    str = "127.0.0.1_30042_1312794771750";
    SplitString(str, "_", &vec);
    EXPECT_EQ("127.0.0.1", vec[0]);
    EXPECT_EQ("30042", vec[1]);
    EXPECT_EQ("1312794771750", vec[2]);

    str = "abc\r\n\r\nbc\raaa\n\n\r\n";
    SplitString(str, "\r\n", &vec);
    ASSERT_EQ(2u, vec.size());
    EXPECT_EQ("abc", vec[0]);
    EXPECT_EQ("bc\raaa\n\n", vec[1]);

    set<string> result;
    str = "aa,dd,dd,aa,cc,aa";
    SplitStringToSet(str, ",", &result);
    ASSERT_EQ(3u, result.size());
    ASSERT_NE(result.count("aa"), 0U);
    ASSERT_NE(result.count("dd"), 0U);
    ASSERT_NE(result.count("cc"), 0U);
}

TEST(String, SplitString) {
    TestSplitString<std::string>();
    TestSplitString<StringPiece>();
}

TEST(String, SplitStringByAnyOf)
{
    string str = "abc\r\n\r\nbc\raaa\n\n\r\n";
    vector<string> vec;
    SplitStringByAnyOf(str, "\r\n", &vec);
    ASSERT_EQ(3u, vec.size());
    EXPECT_EQ("abc", vec[0]);
    EXPECT_EQ("bc", vec[1]);
    EXPECT_EQ("aaa", vec[2]);

    str = "http://www.sina.com.cn\t0.0f\thttp://www.sina.com.cn?a=6";
    SplitStringByAnyOf(str, "\t", &vec);
    ASSERT_EQ(3U, vec.size());
    EXPECT_EQ("http://www.sina.com.cn", vec[0]);
    EXPECT_EQ("0.0f", vec[1]);
    EXPECT_EQ("http://www.sina.com.cn?a=6", vec[2]);
}

TEST(String, SplitStringKeepEmpty)
{
    string str = " ab c  d   efg  end ";
    vector<string> vec;
    SplitStringKeepEmpty(str, " ", &vec);
    ASSERT_EQ(11U, vec.size());

    SplitStringKeepEmpty(str, "  ", &vec);
    ASSERT_EQ(4U, vec.size());
    ASSERT_EQ(" ab c", vec[0]);
    ASSERT_EQ("d", vec[1]);
    ASSERT_EQ(" efg", vec[2]);
    ASSERT_EQ("end ", vec[3]);
}

template <typename StringType>
static void TestSplitLines() {
    vector<StringType> lines;
    StringType str = "abc\r\n" "\n" "bc";
    SplitLines(str, &lines);
    EXPECT_EQ(3U, lines.size());
    EXPECT_EQ("abc", lines[0]);
    EXPECT_EQ("", lines[1]);
    EXPECT_EQ("bc", lines[2]);

    lines.clear();
    SplitLines(str, &lines, true);
    EXPECT_EQ(3U, lines.size());
    EXPECT_EQ("abc\r\n", lines[0]);
    EXPECT_EQ("\n", lines[1]);
    EXPECT_EQ("bc", lines[2]);

    str = "abc\r\n" "\r\n" "bc\r\n" "aaa\n" "\n" "\r\n";
    SplitLines(str, &lines);
    EXPECT_EQ(6U, lines.size());
    EXPECT_EQ("abc", lines[0]);
    EXPECT_EQ("", lines[1]);
    EXPECT_EQ("bc", lines[2]);
    EXPECT_EQ("aaa", lines[3]);
    EXPECT_EQ("", lines[4]);
    EXPECT_EQ("", lines[5]);
}

TEST(String, SplitLines)
{
    TestSplitLines<std::string>();
    TestSplitLines<StringPiece>();
}

TEST(String, ReplaceString) {
    string str = " ab c  d   efghjijkkkkjkk//gj\\*&^xyz  end  ";
    ASSERT_EQ(" ab c  d   efghjijookkjkk//gj\\*&^xyz  end  ", ReplaceFirst(str, "kk", "oo"));
    ASSERT_EQ(" ab c  d   efghjijkkkkjkk//gj\\*&^xyz  end  ", ReplaceFirst(str, "", "xxxx"));
    ASSERT_EQ(" ab c  d   efghjijoooojoo//gj\\*&^xyz  end  ", ReplaceAll(str, "kk", "oo"));
    ASSERT_EQ(" ab c  d   efXhjijkkkkjkk//Xj\\X&^Xyz  end  ", StripString(str, "gx*", 'X'));
    StripString(&str, "gx*", 'X');
    ASSERT_EQ(" ab c  d   efXhjijkkkkjkk//Xj\\X&^Xyz  end  ", str);
    ASSERT_EQ("A_C___B_H", ReplaceAllChars("A-C++/B.H", "/.-+", '_'));
}

TEST(String, InplaceReplaceAll)
{
    std::string s = "AAAB";
    ReplaceAll(&s, "AA", "A");
    EXPECT_EQ("AAB", s);
    s = "AAAB";
    ReplaceAll(&s, "A", "B");
    EXPECT_EQ("BBBB", s);
    s = "a.png";
    ReplaceAll(&s, ".png", ".png.png");
    EXPECT_EQ("a.png.png", s);
}

TEST(String, RemoveSubString)
{
    string str = " abcdefghjijkkkkjkk//gj\\*&^xyz";
    ASSERT_EQ(" abcdefghjijkkkkjkk//gj\\*&^xyz", RemoveContinuousBlank(str));
    ASSERT_EQ(" abcdefghikkkkkk//g\\*&^xyz", RemoveAll(str, "j"));
    ASSERT_EQ(" abcdefgh i kkkk kk//g \\*&^xyz", RemoveAll(str, "j", true));
}

TEST(String, RemoveLineEndingC)
{
    char str[] = "hello, world\r\n\r\n";
    EXPECT_STREQ("hello, world", RemoveLineEnding(str));
    EXPECT_STREQ("hello, world", RemoveLineEnding(str));

    char empty[] = "";
    EXPECT_STREQ("", RemoveLineEnding(empty));
}

template <typename StringType>
static void TestRemoveLineEnding() {
    StringType str = "hello, world\r\n\r\n";
    RemoveLineEnding(&str);
    EXPECT_EQ("hello, world", str);
    RemoveLineEnding(&str);
    EXPECT_EQ("hello, world", str);

    StringType empty = "";
    RemoveLineEnding(&empty);
    EXPECT_EQ("", empty);
}

TEST(String, RemoveLineEnding)
{
    TestRemoveLineEnding<std::string>();
    TestRemoveLineEnding<StringPiece>();
}

TEST(String, JoinStrings)
{
    vector<string> str_vector;
    str_vector.push_back("abc");
    str_vector.push_back("xyz");
    str_vector.push_back("123");
    ASSERT_EQ("abc\txyz\t123", JoinStrings(str_vector, "\t"));
}

TEST(String, JoinSingleString)
{
    vector<string> str_vector;
    str_vector.push_back("abc");
    ASSERT_EQ("abc", JoinStrings(str_vector, "\t"));
}

TEST(String, JoinStringsIterator)
{
    vector<string> str_vector;
    str_vector.push_back("abc");
    str_vector.push_back("xyz");
    str_vector.push_back("123");
    ASSERT_EQ("abc\txyz\t123",
              JoinStrings<vector<string>::const_iterator>(
                  str_vector.begin(), str_vector.end(), "\t"));
}

} // namespace toft
