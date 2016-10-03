/*
 @ 0xCCCCCCCC
*/

#include <memory>

#include "gtest/gtest.h"

#include "kbase/lru_cache.h"

namespace {

template<typename CacheType>
bool CacheOrderingMatch(const CacheType& cache,
                        const std::vector<typename CacheType::key_type>& seq)
{
    return
        std::equal(cache.begin(), cache.end(), seq.begin(),
               [](const typename CacheType::value_type& entry,
                  const typename CacheType::key_type& key)->bool {
            return entry.first == key;
        });
}

}   // namespace

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

    // case: normal insertion

    for (int i = 0; i < 3; ++i) {
        alphabet.Put(candicates[i].first, candicates[i].second);
    }

    EXPECT_EQ(alphabet.size(), 3);
    int idx = 0;
    for (auto it = alphabet.begin(); it != alphabet.end(); ++it, ++idx) {
        EXPECT_EQ(it->first, candicates[idx].first);
        EXPECT_EQ(it->second, candicates[idx].second);
    }

    // case: LRU replacement when running out of free space

    for (int i = 3; i < _countof(candicates); ++i) {
        alphabet.Put(candicates[i].first, candicates[i].second);
    }

    EXPECT_EQ(alphabet.size(), alphabet.max_size());
    idx = 2;
    for (auto it = alphabet.begin(); it != alphabet.end(); ++it, ++idx) {
        EXPECT_EQ(it->first, candicates[idx].first);
        EXPECT_EQ(it->second, candicates[idx].second);
    }

    // case: cache moveable but noncopyable objects

    using AlphabetTable = kbase::LRUHashCache<std::string, std::unique_ptr<int>>;
    AlphabetTable reverse_alphabet(AlphabetTable::NO_AUTO_EVICT);
    reverse_alphabet.Put("A", std::make_unique<int>(65));
    reverse_alphabet.Put("B", std::make_unique<int>(66));

    EXPECT_FALSE(reverse_alphabet.empty());
}

TEST(LRUCacheTest, Get)
{
    using Dict = kbase::LRUTreeCache<int, std::string>;
    Dict dt(Dict::NO_AUTO_EVICT);
    dt.Put(65, "A");
    dt.Put(66, "B");
    dt.Put(67, "C");
    dt.Put(68, "D");

    EXPECT_TRUE(CacheOrderingMatch(dt, {65, 66, 67, 68}));

    // case: entry not found
    {
        auto it = dt.Get(70);
        EXPECT_EQ(it, dt.end());
        EXPECT_TRUE(CacheOrderingMatch(dt, {65, 66, 67, 68}));
    }

    // case: normal access
    {
        auto it = dt.Get(66);
        EXPECT_EQ(it->second, std::string("B"));
        it = dt.Get(68);
        EXPECT_EQ(it->second, std::string("D"));
        EXPECT_FALSE(CacheOrderingMatch(dt, {65, 66, 67, 68}));
        EXPECT_TRUE(CacheOrderingMatch(dt, {65, 67, 66, 68}));
    }
}

TEST(LRUCacheTest, Evict)
{
    using Dict = kbase::LRUTreeCache<int, std::string>;
    Dict dt(Dict::NO_AUTO_EVICT);
    dt.Put(65, "A");
    dt.Put(66, "B");
    dt.Put(67, "C");
    dt.Put(68, "D");

    dt.Evict(2);
    EXPECT_TRUE(CacheOrderingMatch(dt, {67, 68}));

    dt.Evict(2);
    EXPECT_TRUE(dt.empty());
}

TEST(LRUCacheTest, move_semantics)
{
    using Dict = kbase::LRUTreeCache<int, std::string>;

    auto gen = []()->Dict {
        Dict dt(Dict::NO_AUTO_EVICT);
        dt.Put(65, "A");
        dt.Put(66, "B");
        dt.Put(67, "C");
        dt.Put(68, "D");
        return dt;
    };

    Dict new_dt(gen());
    EXPECT_TRUE(CacheOrderingMatch(new_dt, {65, 66, 67, 68}));

    Dict messy_dt(123);
    messy_dt.Put(1, "Its done");
    messy_dt = std::move(new_dt);
    EXPECT_FALSE(messy_dt.auto_evict());
    EXPECT_TRUE(CacheOrderingMatch(messy_dt, {65, 66, 67, 68}));
}