
#include "kbase\path_service.h"

#include <forward_list>
#include <mutex>
#include <unordered_map>

#include "kbase\base_path_provider.h"

using kbase::FilePath;
typedef kbase::PathService::PathKey PathKey;

namespace kbase {

FilePath BasePathProvider(PathKey);

}   // namespace kbase

namespace {

typedef kbase::PathService::ProviderFunc ProviderFunc;
typedef std::unordered_map<PathKey, FilePath> PathMap;

// Both |start| and |end| are used to prevent path keys claimed by different provides
// being overlapped.
struct PathProvider {
    ProviderFunc fn;
    PathKey start;
    PathKey end;
};

// We keep provides in a linked list.
typedef std::forward_list<PathProvider> ProviderChain;

struct PathData {
    ProviderChain providers;
    std::mutex lock;
    PathMap cached_path_table;
    bool cache_disabled;

    PathData()
        : providers {{ PathProvider { kbase::BasePathProvider,
                                      kbase::BASE_PATH_START,
                                      kbase::BASE_PATH_END }}},
          cache_disabled(false)
    {}
};

// TODO: replace with LazyInstance object
PathData g_path_data;

PathData& path_data()
{
    return g_path_data;
}

}   // namespace

namespace kbase {



}   // namespace kbase