/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#define NDEBUG
#include "kbase\string_format.h"
#undef NDEBUG
#include <iostream>
#include <string>

namespace {

using namespace kbase;
using namespace kbase::internal;

template<typename CharT>
bool ComparePlaceholder(const Placeholder<CharT>& p, unsigned i, unsigned pos, const CharT* spec)
{
    return p.index == i && p.pos == pos && p.format_specifier == spec;
}

}

TEST(StringFormatTest, AnalyzeFormatString)
{
    PlaceholderList<char> placeholder_list;
    std::string afmt;

    auto analyzed_fmt = AnalyzeFormatString("blabla {0} {{}} {2} {1:04}", &placeholder_list);
    afmt = "blabla @ {} @ @";
    EXPECT_EQ(afmt, analyzed_fmt);
    ASSERT_EQ(placeholder_list.size(), 3);
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[0], 0, 7, ""));
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[2], 2, 12, ""));
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[1], 1, 14, "04"));

    analyzed_fmt = AnalyzeFormatString("test {0}{1:}{{}} {{{1:#4.3X}}}", &placeholder_list);
    afmt = "test @@{} {@}";
    EXPECT_EQ(afmt, analyzed_fmt);
    ASSERT_EQ(placeholder_list.size(), 3);
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[0], 0, 5, ""));
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[1], 1, 6, ""));
    EXPECT_TRUE(ComparePlaceholder<char>(placeholder_list[2], 1, 11, "#4.3X"));

    EXPECT_THROW(AnalyzeFormatString ("test { 0 }", &placeholder_list), StringFormatSpecifierError);
    EXPECT_THROW(AnalyzeFormatString ("test {}", &placeholder_list), StringFormatSpecifierError);
    EXPECT_THROW(AnalyzeFormatString ("test {{1}", &placeholder_list), StringFormatSpecifierError);
    EXPECT_THROW(AnalyzeFormatString ("test {1 }", &placeholder_list), StringFormatSpecifierError);
    EXPECT_THROW(AnalyzeFormatString ("test {1 {2}", &placeholder_list), StringFormatSpecifierError);
    EXPECT_THROW(AnalyzeFormatString ("test {1:", &placeholder_list), StringFormatSpecifierError);
}

TEST(StringFormatTest, Format)
{
    std::string str, exp_str;

    str = StringFormat("abc {0:0>4X} {1} def {2:.4} {0:+}", 255, "test", 3.14, 123);
    exp_str = "abc 00FF test def 3.1400 +255";
    EXPECT_EQ(exp_str, str);

    // --- a bunch of invalid cases ---.

    EXPECT_ANY_THROW(StringFormat("{0} {1}", 123)); // redundant specifier
    EXPECT_ANY_THROW(StringFormat("{0:>4}", 123));  // fill is missing
    EXPECT_ANY_THROW(StringFormat("{0:.6+4}", 3.1464232));  // specifier order is wrong
    EXPECT_ANY_THROW(StringFormat("{0: >bx}", 123));    // type-mark is more than once
    EXPECT_ANY_THROW(StringFormat("{0: }", 123));    // empty specifier
}