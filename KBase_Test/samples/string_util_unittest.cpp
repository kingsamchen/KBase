/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include "gtest/gtest.h"
#include "kbase/string_util.h"

#include "kbase/auto_reset.h"
#include "kbase/basic_macros.h"

using namespace kbase;

TEST(StringUtilTest, RemoveChars)
{
    const std::string str = "hello world";
    std::string new_str;

    new_str = str;
    RemoveChars(new_str, "ol");
    EXPECT_EQ(new_str, std::string("he wrd"));

    new_str = str;
    RemoveChars(new_str, "tx");
    EXPECT_EQ(str, new_str);

    new_str = str;
    RemoveChars(new_str, "");
    EXPECT_EQ(str, new_str);
}

TEST(StringUtilTest, ReplaceString)
{
    std::string str = "This is a test text for string replacing unittest";

    {
        AutoReset<std::string> guard(&str);
        UNREFED_VAR(guard);

        ReplaceSubstring(str, "test", "t-e-s-t");
        EXPECT_EQ(str, std::string("This is a t-e-s-t text for string replacing unitt-e-s-t"));
        ReplaceSubstring(str, "t-e-s-t", "");
        EXPECT_EQ(str, std::string("This is a  text for string replacing unit"));
        ReplaceSubstring(str, "is", "was", 4);
        EXPECT_EQ(str, std::string("This was a  text for string replacing unit"));
    }

    ReplaceSubstring(str, "is", "ere", 0, false);
    EXPECT_EQ(str, std::string("There is a test text for string replacing unittest"));
}

TEST(StringUtilTest, TrimString)
{
    {
        std::string str = "!$$||hello-world##@@||$$";

        TrimLeadingString(str, "!");
        EXPECT_EQ(std::string("$$||hello-world##@@||$$"), str);

        TrimLeadingString(str, "!");
        EXPECT_EQ(std::string("$$||hello-world##@@||$$"), str);

        TrimString(str, std::string("$$"));
        EXPECT_EQ(std::string("||hello-world##@@||"), str);

        TrimTailingString(str, "||");
        EXPECT_EQ(std::string("||hello-world##@@"), str);
    }

    {
        std::wstring str = L"#@$$@#";

        TrimString(str, L"#@$");
        EXPECT_EQ(std::wstring(L""), str);
    }
}

TEST(StringUtilTest, ContainsOnlyChars)
{
    std::string str = "hello world";
    EXPECT_FALSE(ContainsOnlyChars(str, "aeiou"));

    std::string str2 = "dad";
    EXPECT_TRUE(ContainsOnlyChars(str2, "abcd"));
}

TEST(StringUtilTest, StringToLower)
{
    std::string str = "abcEFGhijKKK";
    EXPECT_EQ(std::string("abcefghijkkk"), (StringToLower(str), str));
}

TEST(StringUtilTest, StartsWithAndEndsWith)
{
    std::string str = "hello world";

    EXPECT_TRUE(StartsWith(str, "hell"));
    EXPECT_FALSE(StartsWith(str, "HELL"));
    EXPECT_TRUE(StartsWith(str, "HELL", false));

    EXPECT_TRUE(StartsWith(std::string("hello"), "hello"));
    EXPECT_TRUE(StartsWith(std::string("hello"), "HELLO", false));
    EXPECT_FALSE(StartsWith(std::string("hell"), "hello"));

    EXPECT_FALSE(EndsWith(str, "old"));
    EXPECT_TRUE(EndsWith(str, "orld"));

    EXPECT_TRUE(EndsWith(std::string("hello"), "hello"));
    EXPECT_TRUE(EndsWith(std::string("hello"), "HellO", false));
    EXPECT_FALSE(EndsWith(std::string("ell"), "hell"));
}

TEST(StringUtilTest, WriteIntoTest)
{
    std::string buffer;
    EXPECT_TRUE(buffer.empty());
    size_t written_size = 12;
    auto ptr = WriteInto(buffer, written_size + 1);
	UNREFED_VAR(ptr);
    EXPECT_EQ(written_size, buffer.size());
}

TEST(StringUtilTest, SplitString)
{
    {
        std::string str = "anything that cannot kill you makes you stronger.\n\tsaid by Bruce Wayne\n";
        std::vector<std::string> exp { "anything", "that", "cannot", "kill", "you", "makes", "you",
                                       "stronger", "said", "by", "Bruce", "Wayne" };
        std::vector<std::string> tokens;
        SplitString(str, " .\n\t", tokens);
        EXPECT_EQ(exp, tokens);
    }

    {
        std::string str = "\r\n\t";
        std::vector<std::string> tokens;
        SplitString(str, "\t\n\r", tokens);
        EXPECT_EQ(decltype(tokens)(), tokens);
    }
}

TEST(StringUtilTest, JoinString)
{
    std::vector<std::string> tokens { "anything", "that", "cannot", "kill", "you", "makes", "you",
                                   "stronger", "said", "by", "Bruce", "Wayne" };
    auto str = JoinString(tokens, " ");
    const std::string exp = "anything that cannot kill you makes you stronger said by Bruce Wayne";
    EXPECT_EQ(exp, str);
}

TEST(StringUtilTest, MatchPatter)
{
    std::string str = "hello world";
    EXPECT_TRUE(MatchPattern(str, "hello*"));
    EXPECT_TRUE(MatchPattern(str, "hello?world"));
}

TEST(StringUtilTest, StringToLowerASCII)
{
    std::string org_str = "HELLO, world";
    std::string turned = StringToLowerASCII(std::move(org_str));
    EXPECT_EQ(turned, std::string("hello, world"));
}