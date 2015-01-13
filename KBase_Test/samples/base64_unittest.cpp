/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\base64.h"

namespace {

const std::pair<std::string, std::string> ciphers[] {
    {"any carnal pleasure.", "YW55IGNhcm5hbCBwbGVhc3VyZS4="},
    {"any carnal pleasure", "YW55IGNhcm5hbCBwbGVhc3VyZQ=="},
    {"any carnal pleasur", "YW55IGNhcm5hbCBwbGVhc3Vy"},
    {"pleasure.", "cGxlYXN1cmUu"},
    {"easure.", "ZWFzdXJlLg=="},
    {"sure.", "c3VyZS4="}
};

}   // namespace

TEST(Base64Test, Encode)
{
    for (const auto& cp : ciphers) {
        auto encoded = kbase::Base64Encode(cp.first);
        EXPECT_EQ(encoded, cp.second);
    }
}