/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include "kbase/base64.h"

namespace kbase {

TEST_CASE("Encoding and decoding", "[Base64]")
{
    const std::pair<std::string, std::string> ciphers[] {
        {"any carnal pleasure.", "YW55IGNhcm5hbCBwbGVhc3VyZS4="},
        {"any carnal pleasure", "YW55IGNhcm5hbCBwbGVhc3VyZQ=="},
        {"any carnal pleasur", "YW55IGNhcm5hbCBwbGVhc3Vy"},
        {"pleasure.", "cGxlYXN1cmUu"},
        {"easure.", "ZWFzdXJlLg=="},
        {"sure.", "c3VyZS4="}
    };

    SECTION("encoding plain text")
    {
        for (const auto& cp : ciphers) {
            auto encoded = Base64Encode(cp.first);
            REQUIRE(encoded == cp.second);
        }
    }

    SECTION("decoding to plain text")
    {
        for (const auto& cp : ciphers) {
            auto decoded = Base64Decode(cp.second);
            REQUIRE(decoded == cp.first);
        }
    }

    SECTION("decoding returns an empty string if the input is invalid")
    {
        std::string s = "aHR0c\\~DovLzEj";
        auto decoded = Base64Decode(s);
        REQUIRE(decoded.empty());
    }
}

}   // namespace kbase
