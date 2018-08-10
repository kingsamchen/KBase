/*
 @ 0xCCCCCCCC
*/

#include <cstring>
#include <iostream>

#include "catch2/catch.hpp"

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
    for (auto& ch : upper_str) {
        ch = static_cast<char>(toupper(ch));
    }

    return upper_str;
}

}   // namespace

namespace kbase {

TEST_CASE("Generating or validating a GUID", "[GUID]")
{
    SECTION("generate a guid")
    {
        auto guid = GenerateGUID();
        REQUIRE_FALSE(guid.empty());
        std::cout << guid << std::endl;
    }

    SECTION("validate a guid")
    {
        auto valid_guid = GenerateGUID();
        REQUIRE(IsGUIDValid(valid_guid, true));
        REQUIRE(IsGUIDv4(valid_guid));

        std::string invalid_guid = "8B6099ED-1517-4735-81F2-3FECEEE56CG8";
        REQUIRE_FALSE(IsGUIDValid(invalid_guid));

        auto upper = UpperASCIIString(valid_guid);
        std::cout << "up-cased: " << upper << std::endl;
        REQUIRE_FALSE(IsGUIDValid(upper, true));
        REQUIRE(IsGUIDValid(upper));
    }
}

}   // namespace kbase
