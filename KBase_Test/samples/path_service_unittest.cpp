
#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\path_service.h"

using namespace kbase;

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
    
}