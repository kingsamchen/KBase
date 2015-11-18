/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\path_service.h"

#include <Windows.h>

#include <cassert>

using namespace kbase;

namespace {

enum PathProviderForTest : PathKey {
    TEST_PATH_START = 100,
    DIR_TEST,
    TEST_PATH_END
};

}   // namespace

TEST(PathServiceTest, GetPath)
{
    // PathService::Get always returns absolute path.
    for (auto key = BASE_PATH_START + 1; key < BASE_PATH_END; ++key) {
        FilePath path = PathService::Get(key);
        EXPECT_FALSE(path.empty());
        EXPECT_FALSE(path.ReferenceParent());
    }
}

TEST(PathServiceTest, RegisterPathProvider)
{
    auto TestPathProvider = [](PathKey key)->FilePath {
        assert(key == DIR_TEST);
        wchar_t buffer[_MAX_PATH+1];
        GetCurrentDirectory(_MAX_PATH + 1, buffer);
        return FilePath(buffer);
    };

    EXPECT_TRUE(PathService::Get(DIR_TEST).empty());

    PathService::RegisterPathProvider(TestPathProvider,
                                      TEST_PATH_START, TEST_PATH_END);
    
    FilePath path = PathService::Get(DIR_TEST);
    EXPECT_FALSE(path.empty());
    EXPECT_FALSE(path.ReferenceParent());
}