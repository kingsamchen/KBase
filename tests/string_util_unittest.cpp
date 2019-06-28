/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include "kbase/auto_reset.h"
#include "kbase/basic_macros.h"
#include "kbase/string_util.h"

namespace kbase {

TEST_CASE("Erasing and removing", "[StringUtil]")
{
    const std::string str = "hello world";
    std::string new_str;

    new_str = str;
    EraseChars(new_str, "ol");
    REQUIRE(new_str == std::string("he wrd"));

    new_str = str;
    EraseChars(new_str, "tx");
    REQUIRE(str == new_str);

    new_str = str;
    EraseChars(new_str, "");
    REQUIRE(str == new_str);

    std::wstring ws = L"hello $%^& world";
    std::wstring nws = EraseCharsCopy(ws, L"$%^&");
    REQUIRE(ws == L"hello $%^& world");
    REQUIRE(nws == L"hello  world");
}

TEST_CASE("Replacing substring", "[StringUtil]")
{
    std::string str = "This is a test text for string replacing unittest";

    SECTION("replace all is by default") {
        ReplaceString(str, "test", "t-e-s-t");
        REQUIRE(str == std::string("This is a t-e-s-t text for string replacing unitt-e-s-t"));
        ReplaceString(str, "t-e-s-t", "");
        REQUIRE(str == std::string("This is a  text for string replacing unit"));
        ReplaceString(str, "is", "was", 4);
        REQUIRE(str == std::string("This was a  text for string replacing unit"));
    }

    SECTION("replace only the first ocurrence") {
        ReplaceString(str, "is", "ere", 0, false);
        REQUIRE(str == std::string("There is a test text for string replacing unittest"));
    }
}

TEST_CASE("Triming", "[StringUtil]")
{
    {
        std::string str = "!$$||hello-world##@@||$$";

        TrimLeadingString(str, "!");
        REQUIRE(std::string("$$||hello-world##@@||$$") == str);

        TrimLeadingString(str, "!");
        REQUIRE(std::string("$$||hello-world##@@||$$") == str);

        TrimString(str, std::string("$$"));
        REQUIRE(std::string("||hello-world##@@||") == str);

        TrimTailingString(str, "||");
        REQUIRE(std::string("||hello-world##@@") == str);
    }

    {
        std::wstring str = L"#@$$@#";

        TrimString(str, L"#@$");
        REQUIRE(std::wstring(L"") == str);
    }

    {
        std::string str1 = "!$$||hello-world##@@||$$";
        std::string str2 = "!$$||hello-world##@@||$$";
        str1 = TrimTailingStringCopy(TrimLeadingStringCopy(str1, "!$|@#"), "!$|@#");
        TrimString(str2, "!$|@#");
        REQUIRE(str1 == str2);
    }
}

TEST_CASE("Contains only", "[StringUtil]")
{
    std::string str = "hello world";
    REQUIRE_FALSE(ContainsOnlyChars(str, "aeiou"));

    std::string str2 = "dad";
    REQUIRE(ContainsOnlyChars(str2, "abcd"));
}

TEST_CASE("Toggling case for ASCII characters", "[StringUtil]")
{
    std::string org_str = "HELLO, world";
    std::string turned = ASCIIStringToLowerCopy(org_str);
    REQUIRE(std::string("hello, world") == turned);
    REQUIRE(std::string("HELLO, WORLD") == ASCIIStringToUpperCopy(turned));
}

TEST_CASE("Case-insensitive comparison for ASCII characters", "[StringUtil]")
{
    REQUIRE(ASCIIStringCompareCaseInsensitive("hello world", "HELLO WORLD") == 0);
    REQUIRE(ASCIIStringCompareCaseInsensitive("JOHNSTON", "John_Henry") != 0);
}

TEST_CASE("Prefix or postfix match", "[StringUtil]")
{
    std::string str = "hello world";

    REQUIRE(StartsWith(str, "hell"));
    REQUIRE_FALSE(StartsWith(str, "HELL"));
    REQUIRE(StartsWith(str, "HELL", CaseMode::ASCIIInsensitive));

    REQUIRE(StartsWith(std::string("hello"), "hello"));
    REQUIRE(StartsWith(std::string("hello"), "HELLO", CaseMode::ASCIIInsensitive));
    REQUIRE_FALSE(StartsWith(std::string("hell"), "hello"));

    REQUIRE_FALSE(EndsWith(str, "old"));
    REQUIRE(EndsWith(str, "orld"));

    REQUIRE(EndsWith(std::string("hello"), "hello"));
    REQUIRE(EndsWith(std::string("hello"), "HellO", CaseMode::ASCIIInsensitive));
    REQUIRE_FALSE(EndsWith(std::string("ell"), "hell"));
}

TEST_CASE("Write raw data into std::string", "[StringUtil]")
{
    std::string buffer;
    REQUIRE(buffer.empty());
    size_t written_size = 12;
    auto ptr = WriteInto(buffer, written_size + 1);
    UNUSED_VAR(ptr);
    REQUIRE(written_size == buffer.size());
}

TEST_CASE("Split a string into a vector of tokens", "[StringUtil]")
{
    {
        std::string str = "anything that cannot kill you makes you stronger.\n\tsaid by Bruce Wayne\n";
        std::vector<std::string> exp { "anything", "that", "cannot", "kill", "you", "makes", "you",
                                       "stronger", "said", "by", "Bruce", "Wayne" };
        std::vector<std::string> tokens;
        SplitString(str, " .\n\t", tokens);
        REQUIRE(exp == tokens);
    }

    {
        std::string str = "\r\n\t";
        std::vector<std::string> tokens;
        SplitString(str, "\t\n\r", tokens);
        REQUIRE(decltype(tokens)() == tokens);
    }
}

TEST_CASE("Join a vector of tokens into a string", "[StringUtil]")
{
    std::vector<std::string> tokens { "anything", "that", "cannot", "kill", "you", "makes", "you",
                                   "stronger", "said", "by", "Bruce", "Wayne" };
    auto str = JoinString(tokens, " ");
    const std::string exp = "anything that cannot kill you makes you stronger said by Bruce Wayne";
    REQUIRE(exp == str);
}

TEST_CASE("String matching", "[StringUtil]")
{
    std::string str = "hello world";
    REQUIRE(MatchPattern(str, "hello*"));
    REQUIRE(MatchPattern(str, "hello?world"));
}

TEST_CASE("Check if a string contains ASCII only", "[StringUtil]")
{
    REQUIRE(IsStringASCIIOnly("hello world!"));
    REQUIRE_FALSE(IsStringASCIIOnly("this is a mix encoding. \xe4\xbd\xa0\xe5\xa5\xbd"));
}

}   // namespace kbase
