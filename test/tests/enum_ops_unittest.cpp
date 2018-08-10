/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include "kbase/basic_types.h"

namespace kbase {

enum class ChangeLevel : unsigned int {
    None = 0,
    Local = 1 << 0,
    Server = 1 << 1,
    All = Local | Server
};

TEST_CASE("Enabled for all enum types once making visible", "[EnumOps]")
{
    using namespace enum_ops;

    ChangeLevel level = ChangeLevel::None;
    level |= ChangeLevel::Local;
    level |= ChangeLevel::Server;

    REQUIRE(level == ChangeLevel::All);
    REQUIRE(enum_cast(level & ChangeLevel::Local) != 0);
    REQUIRE(enum_cast(level & ChangeLevel::Server) != 0);
}

TEST_CASE("No effects for non-enum types", "[EnumOps]")
{
    using namespace enum_ops;

    unsigned int l = 1;
    unsigned int r = 1 << 1;
    auto rv = l | r;
    REQUIRE(3 == rv);

    //std::string s1 = "hello";
    //std::string s2 = "world";
    //s1 | s2;
}

}   // namespace kbase
