/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"
#include "gtest/gtest.h"

#include "kbase/basic_macros.h"
#include "kbase/tokenizer.h"

using kbase::Tokenizer;
using kbase::WTokenizer;

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
        if (!token.empty()) {
            EXPECT_EQ(exp[i], token.ToString());
            ++i;
        }
    }
}