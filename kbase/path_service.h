
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_PATH_SERVICE_H_
#define KBASE_PATH_SERVICE_H_

#include <functional>

#include "kbase\basic_types.h"
#include "kbase\base_path_provider.h"
#include "kbase\files\file_path.h"

namespace kbase {

class PathService {
public:
    typedef std::function<FilePath(PathKey)> ProviderFunc;

    PathService() = delete;

    ~PathService() = delete;

    static FilePath Get(PathKey key);

    static void RegisterPathProvider(ProviderFunc provider);

    static void DisableCache();

    static void EnableCache();
};

}   // namespace kbase

#endif  // KBASE_PATH_SERVICE_H_