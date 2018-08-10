/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include "kbase/basic_macros.h"
#include "kbase/tokenizer.h"

namespace kbase {

TEST_CASE("Constructing tokenizer or token-iterator", "[Tokenizer]")
{
    SECTION("constructing an empty TokenIterator")
    {
        std::string str = "hello, world";
        TokenIterator<char> it(str, str.length(), ", ");
        REQUIRE(it->empty());
    }

    SECTION("constructing tokenizer")
    {
        std::string str = "hello, world";
        Tokenizer tokenizer(str, ", ");
        auto begin = tokenizer.begin();
        auto end = tokenizer.end();
        auto begin_copy = begin;
        auto end_move = std::move(end);
        UNUSED_VAR(begin_copy);
        UNUSED_VAR(end_move);
    }
}

TEST_CASE("Iteration", "[Tokenizer]")
{
    std::string str = "anything that cannot kill you makes you stronger.\n\tsaid by Bruce Wayne\n";
    std::vector<std::string> exp { "anything", "that", "cannot", "kill", "you", "makes", "you",
                                   "stronger", "said", "by", "Bruce", "Wayne" };
    Tokenizer tokenizer(str, " .\n\t");
    size_t i = 0;
    for (auto&& token : tokenizer) {
        REQUIRE(!token.empty());
        REQUIRE(exp[i] == token.ToString());
        ++i;
    }
}

TEST_CASE("None token", "[Tokenizer]")
{
    const char str[] = "\r\n\r\n";

    Tokenizer tokenizer(str, "\r\n");
    REQUIRE(tokenizer.begin() == tokenizer.end());
}

TEST_CASE("String ends with double-delimiter", "[Tokenizer]")
{
    std::string str =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: 30\r\n"
        "Server: Werkzeug/0.12.2 Python/3.6.3\r\n"
        "Date: Thu, 14 Dec 2017 13:10:12 GMT\r\n\r\n";

    Tokenizer tokenizer(str, "\r\n");
    auto count = std::distance(std::next(tokenizer.begin()), tokenizer.end());
    REQUIRE(4 == count);
}

}   // namespace kbase
