/*
 @ 0xCCCCCCCC
*/

#include <vector>

#include "catch2/catch.hpp"

#include "kbase/auto_reset.h"

namespace kbase {

TEST_CASE("Restoring variable's original value", "[AutoReset]")
{
    std::vector<std::string> value {"hello", "world", "kc"};

    SECTION("reset to original value once going out of scope")
    {
        auto vec = value;

        {
            AutoReset<decltype(vec)> value_guard(&vec);
            vec.pop_back();
            vec.push_back("this is a test");
            REQUIRE(value != vec);
        }

        REQUIRE(value == vec);
    }
}

}   // namespace kbase
