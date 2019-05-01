/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include "kbase/string_encoding_conversions.h"

namespace {

std::string utf8_s = "\xe4\xbd\xa0\xe5\xa5\xbd hello chinese test \xe4\xb8\xad\xe6\x96\x87\xe6\xb5\x8b\xe8\xaf\x95";
std::wstring ws = L"\u4f60\u597d hello chinese test \u4e2d\u6587\u6d4b\u8bd5";

}   // namespace

namespace kbase {

TEST_CASE("Convert wide to utf-8", "[StringEncodingConversion]")
{
    auto u8s = WideToUTF8(ws);
    REQUIRE(utf8_s == u8s);
}

TEST_CASE("Convert utf-8 to wide", "[StringEncodingConversion]")
{
    auto wss = UTF8ToWide(utf8_s);
    REQUIRE(ws == wss);
}

TEST_CASE("Conversion between ASCII and Wide", "[StringEncodingConversion]")
{
    std::string ascii = "hello there";
    std::wstring wide = L"hello there";

    SECTION("when convert ascii string to wide string")
    {
        auto result = ASCIIToWide(ascii);
        REQUIRE(result == wide);
    }

    SECTION("when convert wide string to ascii")
    {
        auto result = WideToASCII(wide);
        REQUIRE(result == ascii);
    }
}

}   // namespace kbase
