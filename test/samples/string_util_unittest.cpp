/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

#include "kbase/auto_reset.h"
#include "kbase/basic_macros.h"
#include "kbase/string_util.h"

namespace kbase {

TEST(StringUtilTest, EraseAndRemove)
{
    const std::string str = "hello world";
    std::string new_str;

    new_str = str;
    EraseChars(new_str, "ol");
    EXPECT_EQ(new_str, std::string("he wrd"));

    new_str = str;
    EraseChars(new_str, "tx");
    EXPECT_EQ(str, new_str);

    new_str = str;
    EraseChars(new_str, "");
    EXPECT_EQ(str, new_str);

    std::wstring ws = L"hello $%^& world";
    std::wstring nws = RemoveChars(ws, L"$%^&");
    EXPECT_EQ(ws, L"hello $%^& world");
    EXPECT_EQ(nws, L"hello  world");
}

TEST(StringUtilTest, ReplaceString)
{
    std::string str = "This is a test text for string replacing unittest";

    {
        AutoReset<std::string> guard(&str);
        UNUSED_VAR(guard);

        ReplaceString(str, "test", "t-e-s-t");
        EXPECT_EQ(str, std::string("This is a t-e-s-t text for string replacing unitt-e-s-t"));
        ReplaceString(str, "t-e-s-t", "");
        EXPECT_EQ(str, std::string("This is a  text for string replacing unit"));
        ReplaceString(str, "is", "was", 4);
        EXPECT_EQ(str, std::string("This was a  text for string replacing unit"));
    }

    ReplaceString(str, "is", "ere", 0, false);
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

    {
        std::string str1 = "!$$||hello-world##@@||$$";
        std::string str2 = "!$$||hello-world##@@||$$";
        TrimTailingString(TrimLeadingString(str1, "!$|@#"), "!$|@#");
        TrimString(str2, "!$|@#");
        EXPECT_EQ(str1, str2);
    }
}

TEST(StringUtilTest, ContainsOnlyChars)
{
    std::string str = "hello world";
    EXPECT_FALSE(ContainsOnlyChars(str, "aeiou"));

    std::string str2 = "dad";
    EXPECT_TRUE(ContainsOnlyChars(str2, "abcd"));
}

TEST(StringUtilTest, ToggleASCIIStringCase)
{
    std::string org_str = "HELLO, world";
    std::string turned = ASCIIStringToLower(org_str);
    EXPECT_EQ(std::string("hello, world"), turned);
    EXPECT_EQ(std::string("HELLO, WORLD"), ASCIIStringToUpper(turned));
}

TEST(StringUtilTest, ASCIIStringCompareCaseInsensitive)
{
    EXPECT_TRUE(ASCIIStringCompareCaseInsensitive("hello world", "HELLO WORLD") == 0);
    EXPECT_TRUE(ASCIIStringCompareCaseInsensitive("JOHNSTON", "John_Henry") != 0);
}

TEST(StringUtilTest, StartsWithAndEndsWith)
{
    std::string str = "hello world";

    EXPECT_TRUE(StartsWith(str, "hell"));
    EXPECT_FALSE(StartsWith(str, "HELL"));
    EXPECT_TRUE(StartsWith(str, "HELL", CaseMode::ASCIIInsensitive));

    EXPECT_TRUE(StartsWith(std::string("hello"), "hello"));
    EXPECT_TRUE(StartsWith(std::string("hello"), "HELLO", CaseMode::ASCIIInsensitive));
    EXPECT_FALSE(StartsWith(std::string("hell"), "hello"));

    EXPECT_FALSE(EndsWith(str, "old"));
    EXPECT_TRUE(EndsWith(str, "orld"));

    EXPECT_TRUE(EndsWith(std::string("hello"), "hello"));
    EXPECT_TRUE(EndsWith(std::string("hello"), "HellO", CaseMode::ASCIIInsensitive));
    EXPECT_FALSE(EndsWith(std::string("ell"), "hell"));
}

TEST(StringUtilTest, WriteIntoTest)
{
    std::string buffer;
    EXPECT_TRUE(buffer.empty());
    size_t written_size = 12;
    auto ptr = WriteInto(buffer, written_size + 1);
    UNUSED_VAR(ptr);
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

TEST(StringUtilTest, IsStringASCIIOnly)
{
    EXPECT_TRUE(IsStringASCIIOnly("hello world!"));
    EXPECT_FALSE(IsStringASCIIOnly("this is a mix encoding. \xe4\xbd\xa0\xe5\xa5\xbd"));
}

}   // namespace kbase
