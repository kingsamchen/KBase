/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

#include "kbase/guid.h"

namespace {

bool IsGUIDv4(const std::string& guid)
{
  // The format of GUID version 4 must be xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx,
  // where y is one of [8, 9, A, B].
  return guid[14] == '4' && (guid[19] == '8' || guid[19] == '9' || guid[19] == 'A' ||
         guid[19] == 'a' || guid[19] == 'B' || guid[19] == 'b');
}

std::string UpperASCIIString(const std::string& str)
{
    std::string upper_str(str);
    std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(), toupper);
    return upper_str;
}

}   // namespace

namespace kbase {

TEST(GUIDTest, Generate)
{
    auto guid = GenerateGUID();
    ASSERT_FALSE(guid.empty());
    std::cout << guid << std::endl;
}

TEST(GUIDTest, Validate)
{
    auto valid_guid = GenerateGUID();
    EXPECT_TRUE(IsGUIDValid(valid_guid, true));
    EXPECT_TRUE(IsGUIDv4(valid_guid));

    std::string invalid_guid = "8B6099ED-1517-4735-81F2-3FECEEE56CG8";
    EXPECT_FALSE(IsGUIDValid(invalid_guid));

    auto upper = UpperASCIIString(valid_guid);
    std::cout << "up-cased: " << upper << std::endl;
    EXPECT_FALSE(IsGUIDValid(upper, true));
    EXPECT_TRUE(IsGUIDValid(upper));
}

}   // namespace kbase
