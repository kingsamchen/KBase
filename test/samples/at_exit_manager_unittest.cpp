/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

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

}

// WARNING: The following code about how `AtExitManager` is used is technically wrong,
// and they exist only for test.
// Should **never** use `AtExitManager` like that in product code.

TEST(AtExitManagerTest, Normal)
{
    {
        kbase::AtExitManager exit_manager;
        kbase::AtExitManager::RegisterCallback(std::bind(IncreaseCounter1, 2));
        kbase::AtExitManager::RegisterCallback(std::bind(IncreaseCounter2, 1));
        EXPECT_EQ(g_counter_1, 0);
        EXPECT_EQ(g_counter_2, 0);
    }

    EXPECT_EQ(g_counter_1, 2);
    EXPECT_EQ(g_counter_2, 1);

    ZeroCounters();
}

TEST(AtExitManagerTest, FILO_Order)
{
    kbase::AtExitManager exit_manager;
    kbase::AtExitManager::RegisterCallback(std::bind(IncreaseCounter1, 2));
    kbase::AtExitManager::RegisterCallback([]() {
        EXPECT_EQ(g_counter_1, 0);
        EXPECT_EQ(g_counter_2, 1);
    });
    kbase::AtExitManager::RegisterCallback(std::bind(IncreaseCounter2, 1));
    ASSERT_EQ(g_counter_1, 0);
    ASSERT_EQ(g_counter_2, 0);
}