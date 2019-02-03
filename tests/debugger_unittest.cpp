/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include <iostream>

#include "kbase/debugger.h"

namespace kbase {

TEST_CASE("Check if a debugger is present", "[debugger]")
{
    bool present = IsDebuggerPresent();
    std::cout << "Is now debugger present: " << present << std::endl;
}

}   // namespace kbase
