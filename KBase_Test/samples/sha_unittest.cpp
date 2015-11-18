/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\sha.h"

namespace {

const std::pair<std::string, std::string> sha1_pairs[] {
    {"The quick brown fox jumps over the lazy dog", "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"},
    {"The quick brown fox jumps over the lazy cog", "de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3"}
};

const std::pair<std::string, std::string> sha256_pairs[] {
    {"", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
    {"hello world, this his kc.", "31954a6f1546d181fb45a91a1511065c7dfa2914128e83e8a4afc482e86bd24c"}
};

}   // namespace

TEST(SHATest, SHA1)
{
    for (const auto& p : sha1_pairs) {
        std::string hash = kbase::SHA1String(p.first);
        EXPECT_EQ(p.second, hash);
    }
}

TEST(SHATest, SHA256)
{
    for (const auto& p : sha256_pairs) {
        std::string hash = kbase::SHA256String(p.first);
        EXPECT_EQ(p.second, hash);
    }
}