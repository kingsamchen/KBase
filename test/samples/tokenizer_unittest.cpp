/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

#include "kbase/basic_macros.h"
#include "kbase/tokenizer.h"

namespace kbase {

TEST(TokenizerTest, TokenIterator)
{
    std::string str = "hello, world";
    TokenIterator<char> it(str, str.length(), ", ");
    EXPECT_TRUE(it->empty());
}

TEST(TokenizerTest, Ctor)
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

TEST(TokenizerTest, Iterate)
{
    std::string str = "anything that cannot kill you makes you stronger.\n\tsaid by Bruce Wayne\n";
    std::vector<std::string> exp { "anything", "that", "cannot", "kill", "you", "makes", "you",
                                   "stronger", "said", "by", "Bruce", "Wayne" };
    Tokenizer tokenizer(str, " .\n\t");
    size_t i = 0;
    for (auto&& token : tokenizer) {
        EXPECT_TRUE(!token.empty());
        EXPECT_EQ(exp[i], token.ToString());
        ++i;
    }
}

TEST(TokenizerTest, NoneToken)
{
    const char str[] = "\r\n\r\n";

    Tokenizer tokenizer(str, "\r\n");
    EXPECT_TRUE(tokenizer.begin() == tokenizer.end());
}

TEST(TokenizerTest, DoubleDelimEnd)
{
    std::string str =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: 30\r\n"
        "Server: Werkzeug/0.12.2 Python/3.6.3\r\n"
        "Date: Thu, 14 Dec 2017 13:10:12 GMT\r\n\r\n";

    Tokenizer tokenizer(str, "\r\n");
    auto count = std::distance(std::next(tokenizer.begin()), tokenizer.end());
    EXPECT_EQ(4, count);
}

}   // namespace kbase
