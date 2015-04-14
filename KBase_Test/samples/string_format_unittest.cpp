/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\strings\string_format.h"

TEST(StringFormatTest, tt)
{
    kbase::StringFormat("abc", 123, 13);
}