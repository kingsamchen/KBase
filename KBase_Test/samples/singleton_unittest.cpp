/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\memory\singleton.h"

TEST(SingletonTest, uniqueness)
{
    auto i = kbase::Singleton<int>::instance();
    std::cout << i;
    *i = 0x12345678;
    std::cout << std::hex << *i;
    delete i;
}