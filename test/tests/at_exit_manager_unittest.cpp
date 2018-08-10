/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include "kbase/at_exit_manager.h"

namespace {

int g_counter_1 = 0;
int g_counter_2 = 0;

void IncreaseCounter1(int val)
{
    g_counter_1 += val;
}

void IncreaseCounter2(int val)
{
    g_counter_2 += val;
}

void ZeroCounters()
{
    g_counter_1 = 0;
    g_counter_2 = 0;
}

}   // namespace

namespace kbase {

// WARNING: The following code about how `AtExitManager` is used is technically wrong,
// and they exist only for test.
// Should **never** use `AtExitManager` like that in product code.

TEST_CASE("Executing registered functions when out of scope", "[AtExitManager]")
{
    {
        kbase::AtExitManager exit_manager;
        kbase::AtExitManager::RegisterCallback(std::bind(IncreaseCounter1, 2));
        kbase::AtExitManager::RegisterCallback(std::bind(IncreaseCounter2, 1));
        REQUIRE(g_counter_1 == 0);
        REQUIRE(g_counter_2 == 0);
    }

    REQUIRE(g_counter_1 == 2);
    REQUIRE(g_counter_2 == 1);

    ZeroCounters();
}

TEST_CASE("Functions are registered in FILO fashion", "[AtExitManager]")
{
    kbase::AtExitManager exit_manager;
    kbase::AtExitManager::RegisterCallback(std::bind(IncreaseCounter1, 2));
    kbase::AtExitManager::RegisterCallback([]() {
        REQUIRE(g_counter_1 == 0);
        REQUIRE(g_counter_2 == 1);
    });
    kbase::AtExitManager::RegisterCallback(std::bind(IncreaseCounter2, 1));
    REQUIRE(g_counter_1 == 0);
    REQUIRE(g_counter_2 == 0);
}

}   // namespace kbase
