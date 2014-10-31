
#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\auto_reset.h"

#include <string>
#include <vector>

using namespace kbase;

namespace {

std::vector<std::string> origin {"hello", "world", "kc"};

}   // namespace

TEST(AutoResetTest, AutoReset)
{
    std::vector<std::string> v = origin;
    {
        AutoReset<decltype(v)> value_guard(&v);    
        v.pop_back();
        v.push_back("this is a test");
        EXPECT_NE(origin, v);
    }
    
    EXPECT_EQ(origin, v);
}