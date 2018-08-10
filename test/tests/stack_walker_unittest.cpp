/*
 @ 0xCCCCCCCC
*/

#include <iostream>
#include <string>

#include "catch2/catch.hpp"

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

namespace kbase {

TEST_CASE("Dumping callstack", "[StackWalker]")
{
    auto stack_trace = ::internal::foo();
    std::cout << stack_trace << std::endl;
}

}   // namespace kbase
