/*
 @ 0xCCCCCCCC
*/

#include <memory>

#include "catch2/catch.hpp"

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

TEST_CASE("Construction and move semantics", "[LRUCache]")
{
    SECTION("with capacity limit or no evict")
    {
        LRUCache<int, std::string> non_limited(LRUCache<int, std::string>::NoAutoEvict);
        REQUIRE_FALSE(non_limited.auto_evict());
        REQUIRE(non_limited.max_size() == 0);

        LRUCache<int, std::string, HashMap> ltd(1024);
        REQUIRE(ltd.auto_evict());
        REQUIRE(ltd.max_size() == 1024);
    }

    SECTION("instance is movable")
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
        REQUIRE(CacheOrderingMatch(new_dt, {65, 66, 67, 68}));

        Dict messy_dt(123);
        messy_dt.Put(1, "Its done");
        messy_dt = std::move(new_dt);
        REQUIRE_FALSE(messy_dt.auto_evict());
        REQUIRE(CacheOrderingMatch(messy_dt, {65, 66, 67, 68}));
    }
}

TEST_CASE("Put, get and eviction", "[LRUCache]")
{
    SECTION("put elements into the cache")
    {
        std::pair<int, std::string> candidates[] {
            {65, "A"}, {66, "B"}, {67, "C"}, {68, "D"}, {69, "E"}, {70, "F"}, {71, "G"}
        };

        LRUCache<int, std::string> alphabet(5);
        REQUIRE(alphabet.empty());

        // case: normal insertion

        for (int i = 0; i < 3; ++i) {
            alphabet.Put(candidates[i].first, candidates[i].second);
        }

        REQUIRE(alphabet.size() == 3);
        int idx = 0;
        for (auto it = alphabet.begin(); it != alphabet.end(); ++it, ++idx) {
            REQUIRE(it->first == candidates[idx].first);
            REQUIRE(it->second == candidates[idx].second);
        }

        // case: LRU replacement when running out of free space

        for (size_t i = 3; i < array_size(candidates); ++i) {
            alphabet.Put(candidates[i].first, candidates[i].second);
        }

        REQUIRE(alphabet.size() == alphabet.max_size());
        idx = 2;
        for (auto it = alphabet.begin(); it != alphabet.end(); ++it, ++idx) {
            REQUIRE(it->first == candidates[idx].first);
            REQUIRE(it->second == candidates[idx].second);
        }

        // case: cache movable but non-copyable objects

        using AlphabetTable = LRUCache<std::string, std::unique_ptr<int>, HashMap>;
        AlphabetTable reverse_alphabet(AlphabetTable::NoAutoEvict);
        reverse_alphabet.Put("A", std::make_unique<int>(65));
        reverse_alphabet.Put("B", std::make_unique<int>(66));

        REQUIRE_FALSE(reverse_alphabet.empty());
    }

    SECTION("get cached elements")
    {
        using Dict = LRUCache<int, std::string>;
        Dict dt(Dict::NoAutoEvict);
        dt.Put(65, "A");
        dt.Put(66, "B");
        dt.Put(67, "C");
        dt.Put(68, "D");

        REQUIRE(CacheOrderingMatch(dt, {65, 66, 67, 68}));

        // case: entry not found
        {
            auto it = dt.Get(70);
            REQUIRE(it == dt.end());
            REQUIRE(CacheOrderingMatch(dt, {65, 66, 67, 68}));
        }

        // case: normal access
        {
            auto it = dt.Get(66);
            REQUIRE(it->second == std::string("B"));
            it = dt.Get(68);
            REQUIRE(it->second == std::string("D"));
            REQUIRE_FALSE(CacheOrderingMatch(dt, {65, 66, 67, 68}));
            REQUIRE(CacheOrderingMatch(dt, {65, 67, 66, 68}));
        }
    }

    SECTION("evict oldest element when reaching to capacity limit")
    {
        using Dict = LRUCache<int, std::string>;
        Dict dt(Dict::NoAutoEvict);
        dt.Put(65, "A");
        dt.Put(66, "B");
        dt.Put(67, "C");
        dt.Put(68, "D");

        dt.Evict(2);
        REQUIRE(CacheOrderingMatch(dt, {67, 68}));

        dt.Evict(2);
        REQUIRE(dt.empty());
    }
}

}   // namespace kbase
