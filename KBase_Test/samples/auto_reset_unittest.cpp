
#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\auto_reset.h"

#include <string>
#include <vector>

namespace {

std::vector<std::string> value {"hello", "world", "kc"};

}   // namespace

TEST(AutoResetTest, AutoReset)
{
    auto vec = value;
    {
        kbase::AutoReset<decltype(vec)> value_guard(&vec);
        vec.pop_back();
        vec.push_back("this is a test");
        EXPECT_NE(value, vec);
    }

    EXPECT_EQ(value, vec);
}