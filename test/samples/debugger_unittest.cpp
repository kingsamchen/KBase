/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

#include "kbase/debugger.h"

namespace kbase {

TEST(DebuggerTest, DebuggerPresent)
{
    bool present = IsDebuggerPresent();
    std::cout << "Is now debugger present: " << present << std::endl;
}

}   // namespace kbase
