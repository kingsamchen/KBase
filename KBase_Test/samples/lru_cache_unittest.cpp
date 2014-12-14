/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\memory\lru_cache.h"

#include <string>

TEST(LRUCacheTest, Construction)
{
    kbase::LRUTreeCache<int, std::string> non_limited(kbase::LRUTreeCache<int, std::string>::NO_AUTO_EVICT);
    EXPECT_FALSE(non_limited.auto_evict());
    EXPECT_EQ(non_limited.max_size(), 0);

    kbase::LRUHashCache<int, std::string> ltd(1024);
    EXPECT_TRUE(ltd.auto_evict());
    EXPECT_EQ(ltd.max_size(), 1024);
}

TEST(LRUCacheTest, Put)
{
    std::pair<int, std::string> candicates[] {
        {65, "A"}, {66, "B"}, {67, "C"}, {68, "D"}, {69, "E"}, {70, "F"}, {71, "G"}
    };

    kbase::LRUTreeCache<int, std::string> alphabet(5);
    EXPECT_TRUE(alphabet.empty());

    // normal insertion

    for (int i = 0; i < 3; ++i) {
        alphabet.Put(candicates[i].first, candicates[i].second);
    }

    EXPECT_EQ(alphabet.size(), 3);
    int idx = 0;
    for (auto it = alphabet.begin(); it != alphabet.end(); ++it, ++idx) {
        EXPECT_EQ(it->first, candicates[idx].first);
        EXPECT_EQ(it->second, candicates[idx].second);
    }
    idx = 0;
}