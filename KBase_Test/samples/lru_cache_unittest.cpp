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