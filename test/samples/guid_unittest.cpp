/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\guid.h"

TEST(GUIDTest, Generate)
{
    auto guid = kbase::GenerateGUID();
    ASSERT_FALSE(guid.empty());
    std::cout << guid << std::endl;
}

TEST(GUIDTest, Validate)
{
    auto valid_guid = kbase::GenerateGUID();
    EXPECT_TRUE(kbase::IsGUIDValid(valid_guid));
    std::string invalid_guid = "8B6099ED-1517-4735-81F2-3FECEEE56CG8";
    EXPECT_FALSE(kbase::IsGUIDValid(invalid_guid));
}