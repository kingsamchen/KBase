/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

#include "kbase/environment.h"

TEST(EnvironmentTest, GetVar)
{
    auto var1 = kbase::Environment::GetVar(L"KCPath");
    EXPECT_TRUE(var1.empty());
    auto var2 = kbase::Environment::GetVar(L"OS");
    EXPECT_FALSE(var2.empty());
    EXPECT_EQ(var2, std::wstring(L"Windows_NT"));
}

TEST(EnvironmentTest, HasVar)
{
    using HasVarPair = std::pair<const wchar_t*, bool>;
    HasVarPair pairs[] {
        {L"KCPath", false}, {L"OS", true}, {L"tmp", true}
    };

    for (const auto& p : pairs) {
        EXPECT_EQ(kbase::Environment::HasVar(p.first), p.second);
    }
}

TEST(EnvironmentTest, SetVar)
{
    kbase::Environment::SetVar(L"KCPath", L"D:\\kc\\no1\\king");
    auto value = kbase::Environment::GetVar(L"KCPath");
    EXPECT_EQ(value, L"D:\\kc\\no1\\king");
}

TEST(EnvironmentTest, RemoveVar)
{
    ASSERT_TRUE(kbase::Environment::HasVar(L"KCPath"));
    kbase::Environment::RemoveVar(L"KCPath");
    EXPECT_FALSE(kbase::Environment::HasVar(L"KCPath"));
    // Do no harm.
    kbase::Environment::RemoveVar(L"non-sense");
}

TEST(EnvironmentTest, CurrentEnvTable)
{
    auto table = kbase::Environment::CurrentEnvironmentTable();
    EXPECT_FALSE(table.empty());
}

TEST(EnvironmentTest, BlockFromTable)
{
    kbase::EnvTable table {
        {L"kc", L"kingsleychen"}, {L"fc", L"flowercode"}, {L"cc", L"chichou"}
    };
    const wchar_t src[] = L"cc=chichou\0fc=flowercode\0kc=kingsleychen\0";

    std::wstring block = kbase::Environment::GetEnvironmentBlock(table);
    EXPECT_EQ(block, std::wstring(std::begin(src), std::end(src)));
}