/*
 @ 0xCCCCCCCC
*/

#include <memory>

#include "gtest/gtest.h"

#include "kbase/lru_cache.h"

namespace {

template<typename T, size_t N>
constexpr size_t array_size(const T(&)[N])
{
    return N;
}

template<typename CacheType>
bool CacheOrderingMatch(const CacheType& cache,
                        const std::vector<typename CacheType::key_type>& seq)
{
    return std::equal(cache.begin(), cache.end(), seq.begin(),
                      [](const typename CacheType::value_type &entry,
                         const typename CacheType::key_type &key)->bool {
                          return entry.first == key;
                      });
}

}   // namespace

namespace kbase {

TEST(LRUCacheTest, Construction)
{
    LRUCache<int, std::string> non_limited(LRUCache<int, std::string>::NoAutoEvict);
    EXPECT_FALSE(non_limited.auto_evict());
    EXPECT_EQ(non_limited.max_size(), 0);

    LRUCache<int, std::string, HashMap> ltd(1024);
    EXPECT_TRUE(ltd.auto_evict());
    EXPECT_EQ(ltd.max_size(), 1024);
}

TEST(LRUCacheTest, Put)
{
    std::pair<int, std::string> candidates[] {
            {65, "A"}, {66, "B"}, {67, "C"}, {68, "D"}, {69, "E"}, {70, "F"}, {71, "G"}
    };

    LRUCache<int, std::string> alphabet(5);
    EXPECT_TRUE(alphabet.empty());

    // case: normal insertion

    for (int i = 0; i < 3; ++i) {
        alphabet.Put(candidates[i].first, candidates[i].second);
    }

    EXPECT_EQ(alphabet.size(), 3);
    int idx = 0;
    for (auto it = alphabet.begin(); it != alphabet.end(); ++it, ++idx) {
        EXPECT_EQ(it->first, candidates[idx].first);
        EXPECT_EQ(it->second, candidates[idx].second);
    }

    // case: LRU replacement when running out of free space

    for (int i = 3; i < array_size(candidates); ++i) {
        alphabet.Put(candidates[i].first, candidates[i].second);
    }

    EXPECT_EQ(alphabet.size(), alphabet.max_size());
    idx = 2;
    for (auto it = alphabet.begin(); it != alphabet.end(); ++it, ++idx) {
        EXPECT_EQ(it->first, candidates[idx].first);
        EXPECT_EQ(it->second, candidates[idx].second);
    }

    // case: cache movable but non-copyable objects

    using AlphabetTable = LRUCache<std::string, std::unique_ptr<int>, HashMap>;
    AlphabetTable reverse_alphabet(AlphabetTable::NoAutoEvict);
    reverse_alphabet.Put("A", std::make_unique<int>(65));
    reverse_alphabet.Put("B", std::make_unique<int>(66));

    EXPECT_FALSE(reverse_alphabet.empty());
}

TEST(LRUCacheTest, Get)
{
    using Dict = LRUCache<int, std::string>;
    Dict dt(Dict::NoAutoEvict);
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
    using Dict = LRUCache<int, std::string>;
    Dict dt(Dict::NoAutoEvict);
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
    using Dict = LRUCache<int, std::string>;

    auto gen = []()->Dict {
        Dict dt(Dict::NoAutoEvict);
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

}   // namespace kbase
