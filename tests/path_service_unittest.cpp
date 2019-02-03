/*
 @ 0xCCCCCCCC
*/

#include <cassert>

#include "catch2/catch.hpp"

#include "kbase/basic_macros.h"
#include "kbase/base_path_provider.h"
#include "kbase/path_service.h"

namespace kbase {

enum PathProviderForTest : PathKey {
    TestPathStart = 100,
    DirTest,
    TestPathEnd
};

Path GetTestDirectory()
{
    return Path(PATH_LITERAL("/home"));
}

enum PathProviderForRecursive : PathKey {
    RecursivePathStart = 200,
    DirTestOnCurrent,
    RecursivePathEnd
};

Path GetTestOnCurrentDirectory()
{
    return PathService::Get(DirCurrent).Append(PATH_LITERAL("kbase_test"));
}

TEST_CASE("Query predefined paths from PathService", "[PathService]")
{
    SECTION("query path for a given key")
    {
        // PathService::Get always returns absolute path.
        for (auto key = BasePathStart + 1; key < BasePathEnd; ++key) {
            Path path = PathService::Get(key);
            REQUIRE_FALSE(path.empty());
            REQUIRE_FALSE(path.ReferenceParent());
#if defined(OS_WIN)
            auto path_value = path.AsUTF8();
#else
            auto path_value = path.value();
#endif
            INFO(key << " <-> " << path_value << "\n");
        }
    }

    SECTION("we can register our path providers")
    {
        auto TestPathProvider = [](PathKey key) {
            if (key == DirTest) {
               return GetTestDirectory();
            }

            return Path();
        };

        REQUIRE(PathService::Get(DirTest).empty());

        PathService::RegisterPathProvider(TestPathProvider, TestPathStart, TestPathEnd);

        Path path = PathService::Get(DirTest);
        REQUIRE_FALSE(path.empty());
        REQUIRE_FALSE(path.ReferenceParent());
    }

    SECTION("our path provider can use path service itself")
    {
        auto provider_fn = [](PathKey key) {
            if (key == DirTestOnCurrent) {
                return GetTestOnCurrentDirectory();
            }

            return Path();
        };

        PathService::RegisterPathProvider(provider_fn, RecursivePathStart, RecursivePathEnd);

        Path path;
        REQUIRE_NOTHROW((path = PathService::Get(DirTestOnCurrent)));
    }
}

}   // namespace kbase
