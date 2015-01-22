/*
@ Kingsley Chen
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