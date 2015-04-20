/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\strings\string_format.h"

#include <iostream>
#include <string>

namespace {

using namespace kbase;
using namespace kbase::internal;

template<typename StrT>
bool ComparePlaceholder(const Placeholder<StrT>& p, unsigned i, unsigned pos, StrT spec)
{
    return p.index == i && p.pos == pos && p.format_specifier == spec;
}

}

TEST(StringFormatTest, AnalyzeFormatString)
{

    PlaceholderList<std::string> placeholder_list;
    std::string afmt;

    auto analyzed_fmt = AnalyzeFormatString("blabla {0} {{}} {1:04}", &placeholder_list);
    afmt = "blabla @ {} @";
    EXPECT_EQ(afmt, analyzed_fmt);
    ASSERT_EQ(placeholder_list.size(), 2);
    EXPECT_TRUE(ComparePlaceholder<std::string>(placeholder_list[0], 0, 7, ""));
    EXPECT_TRUE(ComparePlaceholder<std::string>(placeholder_list[1], 1, 12, "04"));

    analyzed_fmt = AnalyzeFormatString("test {0}{1:}{{}} {{{1:#4.3X}}}", &placeholder_list);
    afmt = "test @@{} {@}";
    EXPECT_EQ(afmt, analyzed_fmt);
    ASSERT_EQ(placeholder_list.size(), 3);
    EXPECT_TRUE(ComparePlaceholder<std::string>(placeholder_list[0], 0, 5, ""));
    EXPECT_TRUE(ComparePlaceholder<std::string>(placeholder_list[1], 1, 6, ""));
    EXPECT_TRUE(ComparePlaceholder<std::string>(placeholder_list[2], 1, 11, "#4.3X"));

    EXPECT_THROW(AnalyzeFormatString("test { 0 }", &placeholder_list), StringFormatSpecifierError);
    EXPECT_THROW(AnalyzeFormatString("test {}", &placeholder_list), StringFormatSpecifierError);
    EXPECT_THROW(AnalyzeFormatString("test {{1}", &placeholder_list), StringFormatSpecifierError);
    EXPECT_THROW(AnalyzeFormatString("test {1 }", &placeholder_list), StringFormatSpecifierError);
    EXPECT_THROW(AnalyzeFormatString("test {1 {2}", &placeholder_list), StringFormatSpecifierError);
    EXPECT_THROW(AnalyzeFormatString("test {1:", &placeholder_list), StringFormatSpecifierError);
}

TEST(StringFormatTest, tt)
{
    /*kbase::StringFormat("abc", 123, 13);
    kbase::StringFormat(L"abc", 123, 123);*/
}