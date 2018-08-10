/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include "kbase/string_format.h"

namespace {

using namespace kbase::internal;

template<typename CharT>
bool ComparePlaceholder(const Placeholder<CharT>& p, unsigned i, unsigned pos, const CharT* spec)
{
    return p.index == i && p.pos == pos && p.format_specifier == spec;
}

}   // namespace

namespace kbase {

TEST_CASE("Printf-like interfaces", "[StringFormat]")
{
    REQUIRE(std::string("hello, 0xCC, test 123") == StringPrintf("hello, %s, test %d", "0xCC", 123));
    REQUIRE(std::wstring(L"hello, 0xCC, test 123") == StringPrintf(L"hello, %ls, test %d", L"0xCC", 123));
}

TEST_CASE("Positioning interfaces", "[StringFormat]")
{
    AlwaysCheckFirstInDebug(false);

    SECTION("analyzing format string")
    {
        PlaceholderList<char> placeholder_list;
        std::string afmt;

        auto analyzed_fmt = AnalyzeFormat("blabla {0} {{}} {2} {1:04}", placeholder_list);
        afmt = "blabla @ {} @ @";
        REQUIRE(afmt == analyzed_fmt);
        REQUIRE(placeholder_list.size() == 3);
        REQUIRE(ComparePlaceholder<char>(placeholder_list[0], 0, 7, ""));
        REQUIRE(ComparePlaceholder<char>(placeholder_list[2], 2, 12, ""));
        REQUIRE(ComparePlaceholder<char>(placeholder_list[1], 1, 14, "04"));

        analyzed_fmt = AnalyzeFormat("test {0}{1:}{{}} {{{1:#4.3X}}}", placeholder_list);
        afmt = "test @@{} {@}";
        REQUIRE(afmt == analyzed_fmt);
        REQUIRE(placeholder_list.size() == 3);
        REQUIRE(ComparePlaceholder<char>(placeholder_list[0], 0, 5, ""));
        REQUIRE(ComparePlaceholder<char>(placeholder_list[1], 1, 6, ""));
        REQUIRE(ComparePlaceholder<char>(placeholder_list[2], 1, 11, "#4.3X"));

        REQUIRE_THROWS_AS(AnalyzeFormat("test { 0 }", placeholder_list), FormatError);
        REQUIRE_THROWS_AS(AnalyzeFormat("test {}", placeholder_list), FormatError);
        REQUIRE_THROWS_AS(AnalyzeFormat("test {{1}", placeholder_list), FormatError);
        REQUIRE_THROWS_AS(AnalyzeFormat("test {1 }", placeholder_list), FormatError);
        REQUIRE_THROWS_AS(AnalyzeFormat("test {1 {2}", placeholder_list), FormatError);
        REQUIRE_THROWS_AS(AnalyzeFormat("test {1:", placeholder_list), FormatError);
    }

    SECTION("formating")
    {
        std::string str, exp_str;

        str = StringFormat("abc {0:0>4X} {1} def {2:.4} {0:+}", 255, "test", 3.14, 123);
        exp_str = "abc 00FF test def 3.1400 +255";
        REQUIRE(exp_str == str);

        // --- a bunch of invalid cases ---.

        REQUIRE_THROWS_AS(StringFormat("{0} {1}", 123), FormatError); // redundant specifier
        REQUIRE_THROWS_AS(StringFormat("{0:>4}", 123), FormatError);  // fill is missing
        REQUIRE_THROWS_AS(StringFormat("{0:.6+4}", 3.1464232), FormatError);  // specifier order is wrong
        REQUIRE_THROWS_AS(StringFormat("{0: >bx}", 123), FormatError);    // type-mark is more than once
        REQUIRE_THROWS_AS(StringFormat("{0: }", 123), FormatError);    // empty specifier
    }
}

}   // namespace kbase
