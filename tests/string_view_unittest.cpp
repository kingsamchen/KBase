/*
 @ 0xCCCCCCCC
*/

#include <cstring>
#include <iostream>

#include "catch2/catch.hpp"

#include "kbase/basic_macros.h"
#include "kbase/string_view.h"

namespace kbase {

TEST_CASE("Complex constructions", "[StringView]")
{
    // for default constructor
    {
        StringView empty_view;
        REQUIRE(empty_view.empty());
        REQUIRE(nullptr == empty_view.data());
        REQUIRE(0 == empty_view.length());
    }

    // for BasicStringView(const CharT* str, size_type count)
    {
        const char* s = "hello, world";
        size_t len = strlen(s);
        StringView s_v(s, len);
        REQUIRE_FALSE(s_v.empty());
        std::cout << s_v.data() << std::endl << s_v.length() << std::endl;
        REQUIRE(s == s_v.data());
        REQUIRE(len == s_v.length());

        std::string str = "this is a test text";
        StringView s_v2(str.data(), str.length());
        REQUIRE_FALSE(s_v2.empty());
        std::cout << s_v2.data() << std::endl << s_v2.length() << std::endl;
        REQUIRE(str.data() == s_v2.data());
        REQUIRE(str.length() == s_v2.length());
    }

    // for copy constructor
    {
        std::string str = "this is a test text";
        StringView s_v(str.data(), str.length());
        StringView s_v_copy(s_v);
        REQUIRE(s_v.data() == s_v_copy.data());
        REQUIRE(s_v.length() == s_v_copy.length());
    }

    // for BasicStringView(const CharT* str)
    {
        const char* s = "hello, world";
        size_t len = strlen(s);
        StringView s_v(s);
        REQUIRE(s == s_v.data());
        REQUIRE(len == s_v.length());
    }

    // for BasicStringView(const std::basic_string<CharT>& str)
    {
        std::string str = "this is a test text";
        StringView s_v(str);
        REQUIRE(str.data() == s_v.data());
        REQUIRE(str.length() == s_v.length());
    }

    // compile-time evaluations.
    {
        constexpr StringView s1;
        constexpr bool empty = s1.empty();
        REQUIRE(empty);

        constexpr StringView s2 = "this is a test text";
        constexpr bool not_empty = s2.empty();
        REQUIRE_FALSE(not_empty);
        constexpr char ft = s2.front();
        constexpr char bk = s2.back();
        constexpr auto sub = s2.substr(5);
        constexpr const char* ss = sub.data();

        UNUSED_VAR(ft);
        UNUSED_VAR(bk);
        UNUSED_VAR(ss);
    }
}

TEST_CASE("Assignment", "[StringView]")
{
    StringView v = "Hello, world";
    StringView v_ex = "this is a test text";
    REQUIRE_FALSE(v.length() == v_ex.length());
    v = v_ex;
    REQUIRE(v.length() == v_ex.length());
}

TEST_CASE("Max size", "[StringView]")
{
    StringView view;
    auto max_size = std::numeric_limits<size_t>::max();
    REQUIRE(max_size == view.max_size());
}

TEST_CASE("Iterator usages", "[StringView]")
{
    const char* raw_str = "hello world";
    StringView view = raw_str;
    std::string str(view.begin(), view.end());
    std::cout << str << std::endl;
    REQUIRE(std::string(raw_str) == str);

    std::string rstr(view.rbegin(), view.rend());
    std::cout << rstr << std::endl;
    std::reverse(str.begin(), str.end());
    REQUIRE(str == rstr);
}

TEST_CASE("Accessing elements", "[StringView]")
{
    StringView view = "hello world";

    // for operator[]
    REQUIRE('h' == view[0]);
    REQUIRE('d' == view[view.length() - 1]);

    // for at
    REQUIRE('h' == view.at(0));
    REQUIRE('d' == view.at(view.length() - 1));
    REQUIRE_THROWS_AS(view.at(view.size()), std::out_of_range);

    // for front
    REQUIRE('h' == view.front());

    // for back
    REQUIRE('d' == view.back());
}

TEST_CASE("Mutating a view", "[StringView]")
{
    StringView view = "hello world";
    size_t space_pos = std::distance(view.begin(), std::find(view.begin(), view.end(), ' '));
    std::cout << space_pos << std::endl;
    REQUIRE(space_pos <= view.length());

    auto v_prefix = view;
    v_prefix.RemovePrefix(space_pos + 1);
    REQUIRE(std::string("world") == std::string(v_prefix.data(), v_prefix.length()));

    auto v_suffix = view;
    v_suffix.RemoveSuffix(view.length() - space_pos);
    REQUIRE(std::string("hello") == std::string(v_suffix.data(), v_suffix.length()));

    StringView v_1 = "hello";
    StringView v_2 = "world";
    v_1.swap(v_2);
    REQUIRE(std::string("world") == std::string(v_1.data(), v_1.length()));
    REQUIRE(std::string("hello") == std::string(v_2.data(), v_2.length()));
}

TEST_CASE("To std string", "[StringView]")
{
    std::string str = "hello, world";
    StringView view = str;
    REQUIRE(str == view.ToString());

    StringView empty_view;
    REQUIRE(std::string() == empty_view.ToString());
}

TEST_CASE("Copying", "[StringView]")
{
    StringView view = "hello, world";
    std::vector<char> buf(32);

    {
        size_t count_copied = view.copy(buf.data(), 5);
        REQUIRE(5 == count_copied);
        REQUIRE(strcmp("hello", buf.data()) == 0);
    }

    buf.clear(); buf.resize(32);
    {
        size_t count_copied = view.copy(buf.data(), 10, 7);
        REQUIRE(10 != count_copied);
        REQUIRE(strcmp("world", buf.data()) == 0);
    }

    buf.clear(); buf.resize(32);
    {
        REQUIRE_THROWS_AS(view.copy(buf.data(), 5, view.length() + 2), std::out_of_range);
    }
}

TEST_CASE("Substring", "[StringView]")
{
    StringView view = "hello, world";

    REQUIRE(std::string(", world") == view.substr(5).ToString());
    REQUIRE(std::string("hello") == view.substr(0, 5).ToString());
    REQUIRE_THROWS_AS(view.substr(view.length() + 2), std::out_of_range);
}

TEST_CASE("Comparisons", "[StringView]")
{
    StringView v_1 = "hello";
    StringView v_2 = "hell";
    REQUIRE(v_1.compare(v_2) > 0);

    REQUIRE(v_1.compare(0, 4, v_2) == 0);

    REQUIRE(v_1.compare(2, 2, v_2, 2, 2) == 0);

    REQUIRE(v_1.compare("he") > 0);

    REQUIRE(v_1.compare(0, 2, "he") == 0);

    REQUIRE(v_1.compare(0, 2, "hell", 2) == 0);
}

TEST_CASE("Finding operations", "[StringView]")
{
    SECTION("find and rfind")
    {
        StringView view = "hello, world";
        StringView view_ex = "where there is a will, there is a way";
        StringView view_ng = "This is a string";
        auto npos = decltype(view)::npos;

        // for find
        REQUIRE(0 == view.find("hell"));
        REQUIRE(8 == view.find("or"));
        REQUIRE(8 == view.find("or", 7));
        REQUIRE(npos == view.find("joke"));
        REQUIRE(npos == view.find("world", 10));
        REQUIRE(4 == view.find('o'));
        REQUIRE(0 == view.find("here", 0, 2));

        // for rfind
        REQUIRE(10 == view.rfind('l'));
        REQUIRE(npos == view.rfind('k'));
        REQUIRE(23 == view_ex.rfind("there"));
        REQUIRE(17 == view_ex.rfind("will", 22));
        REQUIRE(5 == view_ng.rfind("is"));
        REQUIRE(2 == view_ng.rfind("is", 4));
    }

    SECTION("find_first_of and find_last_of")
    {
        StringView view = "where there is a will, there is a way";
        auto npos = decltype(view)::npos;

        // for find_first_of
        REQUIRE(5 == view.find_first_of(" ,"));
        REQUIRE(2 == view.find_first_of("aeiou"));

        // for find_last_of
        StringView view_1 = "abcd-1234-abcd-1234";
        REQUIRE(13 == view_1.find_last_of("d", 14));
        REQUIRE(npos == view_1.find_last_of("x"));

        StringView view_2 = "ABCD-1234-ABCD-1234";
        REQUIRE(11 == view_2.find_last_of("B1", 12));
        REQUIRE(16 == view_2.find_last_of("D2"));

        StringView view_3 = "456-EFG-456-EFG";
        REQUIRE(4 == view_3.find_last_of("5E", 8));

        StringView view_4 = "12-ab-12-ab";
        REQUIRE(4 == view_4.find_last_of("ba3", 8));
        REQUIRE(9 == view_4.find_last_of("a2"));
    }

    SECTION("find not of")
    {
        {
            StringView view_1 = "xddd-1234-abcd";
            REQUIRE(4 == view_1.find_first_not_of("d", 2));
            REQUIRE(1 == view_1.find_first_not_of("x"));

            StringView view_2 = "BBB-1111";
            REQUIRE(StringView::npos == view_2.find_first_not_of("B1", 6));
            REQUIRE(3 == view_2.find_first_not_of("B2"));

            StringView view_3 = "444-555-GGG";
            REQUIRE(3 == view_3.find_first_not_of("45G"));

            StringView view_4 = "12-ab-12-ab";
            REQUIRE(5 == view_4.find_first_not_of("ba3", 5));
            REQUIRE(2 == view_4.find_first_not_of("12"));
        }

        {
            StringView view_1 = "dddd-1dd4-abdd";
            REQUIRE(5 == view_1.find_last_not_of("d", 7));
            REQUIRE(11 == view_1.find_last_not_of("d"));

            StringView view_2 = "BBB-1111";
            REQUIRE(3 == view_2.find_last_not_of("B1", 6));
            REQUIRE(StringView::npos == view_2.find_last_not_of("B-1"));

            StringView view_3 = "444-555-GGG";
            auto pos1 = view_3.find_last_not_of("45G");
            INFO("pos1 = " << pos1);
            REQUIRE(7 == pos1);
            auto pos2 = view_3.find_last_not_of("45G", 6, 3);
            INFO("pos2 = " << pos2);
            REQUIRE(3 == pos2);

            StringView view_4 = "12-ab-12-ab";
            REQUIRE(1 == view_4.find_last_not_of("b-a", 5));
            REQUIRE(10 == view_4.find_last_not_of("12"));
        }
    }
}

TEST_CASE("Operator comparisons", "[StringUtil]")
{
    StringView view_1 = "abc";
    StringView view_2 = "adc";
    REQUIRE_FALSE(view_1 == view_2);
    REQUIRE(view_1 != view_2);

    std::string str_1 = "abc";
    const char kStr[] = "def";
    REQUIRE(view_1 == str_1);
    REQUIRE(view_2 != kStr);
}

TEST_CASE("Output to stream", "[StringUtil]")
{
    StringView v = "hello world";
    WStringView w = L"test text";
    std::cout << v << std::endl;
    std::wcout << w << std::endl;
}

TEST_CASE("Hash support", "[StringUtil]")
{
    StringView v = "hello world";
    WStringView w = L"hello world";
    StringView vx = "This is a test text";
    WStringView wx = L"This is a test text";

    REQUIRE(std::hash<StringView>()(v) == std::hash<StringView>()(v));
    REQUIRE(std::hash<WStringView>()(w) == std::hash<WStringView>()(w));
    REQUIRE(std::hash<StringView>()(v) != std::hash<StringView>()(vx));
    REQUIRE(std::hash<WStringView>()(w) != std::hash<WStringView>()(wx));
}

}   // namespace kbase
