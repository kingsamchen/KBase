/*
 @ 0xCCCCCCCC
*/

#include <cassert>

#include "gtest/gtest.h"

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
    return Path(PATH_LITERAL("Test"));
}

enum PathProviderForRecursive : PathKey {
    RecursivePathStart = 200,
    DirTestOnCurrent,
    RecursivePathEnd
};

Path GetTestOnCurrentDirectory()
{
    return PathService::Get(DirCurrent).Append(PATH_LITERAL("Test"));
}

TEST(PathServiceTest, GetPath)
{
    // PathService::Get always returns absolute path.
    for (auto key = BasePathStart + 1; key < BasePathEnd; ++key) {
        Path path = PathService::Get(key);
        EXPECT_FALSE(path.empty());
        EXPECT_FALSE(path.ReferenceParent());
        std::cout << "Path key " << key << " passed\n";
    }
}

TEST(PathServiceTest, RegisterPathProvider)
{
    auto TestPathProvider = [](PathKey key) {
        if (key == DirTest) {
           return GetTestDirectory();
        }

        return Path();
    };

    EXPECT_TRUE(PathService::Get(DirTest).empty());

    PathService::RegisterPathProvider(TestPathProvider, TestPathStart, TestPathEnd);

    Path path = PathService::Get(DirTest);
    EXPECT_FALSE(path.empty());
    EXPECT_FALSE(path.ReferenceParent());
}

TEST(PathServiceTest, RecursiveGet)
{
    auto provider_fn = [](PathKey key) {
        if (key == DirTestOnCurrent) {
            return GetTestOnCurrentDirectory();
        }

        return Path();
    };

    PathService::RegisterPathProvider(provider_fn, RecursivePathStart, RecursivePathEnd);

    Path path;
    EXPECT_NO_THROW({
        path = PathService::Get(DirTestOnCurrent);
    });
}

}   // namespace kbase
