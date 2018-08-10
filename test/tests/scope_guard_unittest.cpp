/*
 @ 0xCCCCCCCC
*/

#include <vector>

#include "catch2/catch.hpp"

#include "kbase/scope_guard.h"

namespace kbase {

TEST_CASE("Use macro ON_SCOPE_EXIT", "[ScopeGuard]")
{
    int* ptr = new int(1);
    {
        REQUIRE(ptr != nullptr);
        *ptr = 123;
        ON_SCOPE_EXIT {
            delete  ptr;
            ptr = nullptr;
        };
    }

    REQUIRE(nullptr == ptr);
}

TEST_CASE("Use MakeScopedGuard function to explicitly control guard", "[ScopeGuard]")
{
    std::vector<int> v {1, 3, 5};

    {
        auto guard = MAKE_SCOPE_GUARD { v.clear(); };
        if (v.size() < 5) {
            guard.Dismiss();
        }
    }

    REQUIRE(v.size() == 3);

    v.push_back(7);
    v.push_back(9);

    {
        auto guard = MAKE_SCOPE_GUARD { v.clear(); };
        if (v.size() < 5) {
            guard.Dismiss();
        }
    }

    REQUIRE(v.empty());
}

}   // namespace kbase
