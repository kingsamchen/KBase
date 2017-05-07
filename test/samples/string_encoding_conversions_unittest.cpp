/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

#include "kbase/string_encoding_conversions.h"

namespace {

std::string utf8_s = "\xe4\xbd\xa0\xe5\xa5\xbd hello chinese test \xe4\xb8\xad\xe6\x96\x87\xe6\xb5\x8b\xe8\xaf\x95";
std::wstring ws = L"\u4f60\u597d hello chinese test \u4e2d\u6587\u6d4b\u8bd5";

}   // namespace

namespace kbase {

TEST(StringEncodingConversionTest, W2U8)
{
    auto u8s = WideToUTF8(ws);
    EXPECT_EQ(utf8_s, u8s);
}

TEST(StringEncodingConversionTest, U82W)
{
    auto wss = UTF8ToWide(utf8_s);
    EXPECT_EQ(ws, wss);
}

}   // namespace kbase
