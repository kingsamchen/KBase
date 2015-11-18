/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_PATH_SERVICE_H_
#define KBASE_PATH_SERVICE_H_

#include <functional>

#include "kbase\basic_types.h"
#include "kbase\base_path_provider.h"
#include "kbase\file_path.h"

namespace kbase {

class PathService {
public:
    typedef std::function<FilePath(PathKey)> ProviderFunc;

    PathService() = delete;

    ~PathService() = delete;

    // Returns the absolute path corresponding to the key if the function succeeds.
    // Returns an empty file path if no matching was found.
    static FilePath Get(PathKey key);

    // Users can register their own path provider along with a bunch of new path keys.
    // By default, only in debug mode does this function internally do key overlapping
    // checking.
    // WARNING: The provider itself must not call |PathService::Get|.
    static void RegisterPathProvider(ProviderFunc provider,
                                     PathKey start, PathKey end);

    // Disables internal cache.
    static void DisableCache();

    // Enables internal cache.
    static void EnableCache();
};

}   // namespace kbase

#endif  // KBASE_PATH_SERVICE_H_