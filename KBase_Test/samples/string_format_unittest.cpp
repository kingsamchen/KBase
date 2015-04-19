/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\strings\string_format.h"

#include <iostream>

namespace {

}

TEST(StringFormatTest, AnalyzeFormatString)
{
    using namespace kbase::internal;
    PlaceholderList<std::string> placeholder_list;
    auto analyzed_fmt = AnalyzeFormatString("blabla {0} {{}} {1:04}", &placeholder_list);
    std::cout << analyzed_fmt << std::endl;
}

TEST(StringFormatTest, tt)
{
    /*kbase::StringFormat("abc", 123, 13);
    kbase::StringFormat(L"abc", 123, 123);*/
}