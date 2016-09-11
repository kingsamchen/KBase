/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include <vector>

#include "gtest/gtest.h"
#include "kbase/scope_guard.h"

TEST(ScopeGuardTest, OnScopeExit)
{
    int* ptr = new int(1);
    {
        ASSERT_TRUE(ptr != nullptr);
        *ptr = 123;
        ON_SCOPE_EXIT {
            delete  ptr;
            ptr = nullptr;
        };
    }

    EXPECT_EQ(nullptr, ptr);
}

TEST(ScopeGuardTest, MakeScopeGuard)
{
    std::vector<int> v {1, 3, 5};

    {
        auto guard = MAKE_SCOPE_GUARD { v.clear(); };
        if (v.size() < 5) {
            guard.Dismiss();
        }
    }

    ASSERT_TRUE(v.size() == 3);

    v.push_back(7);
    v.push_back(9);

    {
        auto guard = MAKE_SCOPE_GUARD { v.clear(); };
        if (v.size() < 5) {
            guard.Dismiss();
        }
    }

    EXPECT_TRUE(v.empty());
}