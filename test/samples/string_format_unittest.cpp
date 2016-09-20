/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include "gtest/gtest.h"

#if !defined(NDEBUG)
#define NDEBUG
#include "kbase/string_format.h"
#endif

namespace {

using namespace kbase;
using namespace kbase::internal;

template<typename CharT>
bool ComparePlaceholder(const Placeholder<CharT>& p, unsigned i, unsigned pos, const CharT* spec)
{
    return p.index == i && p.pos == pos && p.format_specifier == spec;
}

}

TEST(StringFormatTest, StringPrintfSeries)
{
    EXPECT_EQ(std::string("hello, 0xCC, test 123"), StringPrintf("hello, %s, test %d", "0xCC", 123));
    EXPECT_EQ(std::wstring(L"hello, 0xCC, test 123"), StringPrintf(L"hello, %s, test %d", L"0xCC", 123));
}

TEST(StringFormatTest, AnalyzeFormatString)
{
    PlaceholderList<char> placeholder_list;
    std::string afmt;

    auto analyzed_fmt = AnalyzeFormat("blabla {0} {{}} {2} {1:04}", placeholder_list);
    afmt = "blabla @ {} @ @";
    EXPECT_EQ(afmt, analyzed_fmt);
    ASSERT_EQ(placeholder_list.size(), 3);
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[0], 0, 7, ""));
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[2], 2, 12, ""));
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[1], 1, 14, "04"));

    analyzed_fmt = AnalyzeFormat("test {0}{1:}{{}} {{{1:#4.3X}}}", placeholder_list);
    afmt = "test @@{} {@}";
    EXPECT_EQ(afmt, analyzed_fmt);
    ASSERT_EQ(placeholder_list.size(), 3);
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[0], 0, 5, ""));
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[1], 1, 6, ""));
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[2], 1, 11, "#4.3X"));

    EXPECT_THROW(AnalyzeFormat("test { 0 }", placeholder_list), FormatError);
    EXPECT_THROW(AnalyzeFormat("test {}", placeholder_list), FormatError);
    EXPECT_THROW(AnalyzeFormat("test {{1}", placeholder_list), FormatError);
    EXPECT_THROW(AnalyzeFormat("test {1 }", placeholder_list), FormatError);
    EXPECT_THROW(AnalyzeFormat("test {1 {2}", placeholder_list), FormatError);
    EXPECT_THROW(AnalyzeFormat("test {1:", placeholder_list), FormatError);
}

TEST(StringFormatTest, Format)
{
    std::string str, exp_str;

    str = StringFormat("abc {0:0>4X} {1} def {2:.4} {0:+}", 255, "test", 3.14, 123);
    exp_str = "abc 00FF test def 3.1400 +255";
    EXPECT_EQ(exp_str, str);

    // --- a bunch of invalid cases ---.

    EXPECT_THROW(StringFormat("{0} {1}", 123), FormatError); // redundant specifier
    EXPECT_THROW(StringFormat("{0:>4}", 123), FormatError);  // fill is missing
    EXPECT_THROW(StringFormat("{0:.6+4}", 3.1464232), FormatError);  // specifier order is wrong
    EXPECT_THROW(StringFormat("{0: >bx}", 123), FormatError);    // type-mark is more than once
    EXPECT_THROW(StringFormat("{0: }", 123), FormatError);    // empty specifier
}