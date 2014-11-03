/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\strings\string_util.h"

#include <string>

#include "kbase\auto_reset.h"

using namespace kbase;

TEST(StringUtilTest, RemoveChars)
{
    std::string str = "hello world";
    std::string new_str;
    
    EXPECT_TRUE(RemoveChars(str, "ol", &new_str));
    EXPECT_EQ(new_str, std::string("he wrd"));
    
    EXPECT_FALSE(RemoveChars(str, "tx", &new_str));
    EXPECT_EQ(str, new_str);
    
    EXPECT_FALSE(RemoveChars(str, "", &new_str));
    EXPECT_EQ(str, new_str);

    EXPECT_TRUE(RemoveChars(str, "ol", &str));
    EXPECT_EQ(str, std::string("he wrd"));
}

TEST(StringUtilTest, ReplaceString)
{
    std::string str = "This is a test text for string replacing unittest";

    {
        AutoReset<std::string> guard(&str);
        ReplaceSubstring(&str, "test", "t-e-s-t");
        EXPECT_EQ(str, std::string("This is a t-e-s-t text for string replacing unitt-e-s-t"));
        ReplaceSubstring(&str, "t-e-s-t", "");
        EXPECT_EQ(str, std::string("This is a  text for string replacing unit"));
        ReplaceSubstring(&str, "is", "was", 4);
        EXPECT_EQ(str, std::string("This was a  text for string replacing unit"));
    }

    ReplaceFirstSubstring(&str, "is", "ere");
    EXPECT_EQ(str, std::string("There is a test text for string replacing unittest"));
}

TEST(StringUtilTest, TrimString)
{
    std::string str = "!$$||hello-world##@@||$$";
    
    EXPECT_TRUE(TrimLeadingStr(str, "!", &str));
    EXPECT_EQ(str, std::string("$$||hello-world##@@||$$"));
    EXPECT_FALSE(TrimTailingStr(str, "!", &str));
    EXPECT_EQ(str, std::string("$$||hello-world##@@||$$"));

    EXPECT_TRUE(TrimString(str, std::string("$$"), &str));
    EXPECT_EQ(str, std::string("||hello-world##@@||"));

    EXPECT_TRUE(TrimTailingStr(str, "||", &str));
    EXPECT_EQ(str, std::string("||hello-world##@@"));
}

TEST(StringUtilTest, ContainsOnlyChars)
{
    std::string str = "hello world";
    EXPECT_FALSE(ContainsOnlyChars(str, "aeiou"));
    
    std::string str2 = "dad";
    EXPECT_TRUE(ContainsOnlyChars(str2, "abcd"));
}

TEST(StringUtilTest, StartsWithAndEndsWith)
{
    std::string str = "hello world";
    
    EXPECT_TRUE(StartsWith(str, "hell"));
    EXPECT_FALSE(StartsWith(str, "HELL"));
    EXPECT_TRUE(StartsWith(str, "HELL", false));

    EXPECT_FALSE(EndsWith(str, "old"));
    EXPECT_TRUE(EndsWith(str, "orld"));
}

TEST(StringUtilTest, MatchPatter)
{
    std::string str = "hello world";
    EXPECT_TRUE(MatchPattern(str, "hello*"));
    EXPECT_TRUE(MatchPattern(str, "hello?world"));
}