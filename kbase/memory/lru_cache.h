
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_MEMORY_LRU_CACHE_H_
#define KBASE_MEMORY_LRU_CACHE_H_

#include <map>
#include <unordered_map>

namespace kbase {

template<typename Key, typename Value>
struct TreeMap {
    using Map = std::map<Key, Value>;
};

template<typename Key, typename Value>
struct HashMap {
    using Map = std::unordered_map<Key, Value>;
};

template<typename Key, typename Value, template<typename, typename> class Map>
class LRUCache {
public:
    using key_type = Key;
    using value_type = std::pair<const Key, Value>;

private:
    using CachedEntryList = std::list<value_type>;
    using KeyTable = typename Map<Key, typename CachedEntryList::iterator>::Map;

public:
    using size_type = size_t;
    using iterator = typename CachedEntryList::iterator;
    using const_iterator = typename CachedEntryList::const_iterator;
    using reverse_iterator = typename CachedEntryList::reverse_iterator;
    using const_reverse_iterator = typename CachedEntryList::const_reverse_iterator;
};

}   // namespace kbase

#endif  // KBASE_MEMORY_LRU_CACHE_H_