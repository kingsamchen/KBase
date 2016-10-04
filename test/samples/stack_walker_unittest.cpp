/*
 @ 0xCCCCCCCC
*/

#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "kbase/stack_walker.h"

namespace internal {

std::string bazinga(std::string& log)
{
    log += " -> bazinga";
    std::cout << log << std::endl;
    kbase::StackWalker stack_walker;
    auto stack_trace = stack_walker.CallStackToString();
    return stack_trace;
}

std::string bar(std::string& log)
{
    log += " -> bar";
    return bazinga(log);
}

std::string foo()
{
    std::string log("foo");
    return bar(log);
}

}    // namespace internal

TEST(StackWalkerTest, DumpCallStack)
{
    using namespace internal;
    auto stack_trace = foo();
    std::cout << stack_trace << std::endl;
}
