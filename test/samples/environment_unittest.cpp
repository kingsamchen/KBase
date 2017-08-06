/*
 @ 0xCCCCCCCC
*/

#include "gtest/gtest.h"

#include "kbase/environment.h"
#include "kbase/string_util.h"

namespace kbase {

TEST(EnvironmentTest, GetVar)
{
    EXPECT_TRUE(Environment::GetVar(ENV_LITERAL("KCPath")).empty());
    EXPECT_FALSE(Environment::GetVar(ENV_LITERAL("PATH")).empty());
}

TEST(EnvironmentTest, HasVar)
{
    using HasVarPair = std::pair<Environment::StringType, bool>;

    HasVarPair pairs[] {
        {ENV_LITERAL("KCPath"), false},
        {ENV_LITERAL("PATH"), true},
#if defined(OS_WIN)
        {ENV_LITERAL("TMP"), true}
#else
        {ENV_LITERAL("HOME"), true}
#endif
    };

    for (const auto& p : pairs) {
        EXPECT_EQ(p.second, Environment::HasVar(p.first.c_str()));
    }
}

TEST(EnvironmentTest, SetVar)
{
    const Environment::CharType* env_key = ENV_LITERAL("KCPath");
    Environment::StringType env_value = ENV_LITERAL("D:\\kc\\no1\\king");

    ASSERT_FALSE(Environment::HasVar(env_key));
    Environment::SetVar(env_key, env_value);
    auto value = Environment::GetVar(env_key);
    EXPECT_EQ(value, env_value);

    Environment::SetVar(env_key, Environment::StringType());
    EXPECT_TRUE(Environment::HasVar(env_key));
    value = Environment::GetVar(env_key);
    EXPECT_TRUE(value.empty());
}

TEST(EnvironmentTest, RemoveVar)
{
    const Environment::CharType* env_key = ENV_LITERAL("KCPath");

    ASSERT_TRUE(Environment::HasVar(env_key));
    Environment::RemoveVar(env_key);
    EXPECT_FALSE(Environment::HasVar(env_key));

    // Do no harm.
    Environment::RemoveVar(ENV_LITERAL("non-sense"));
}

TEST(EnvironmentTest, CurrentEnvTable)
{
    auto table = Environment::CurrentEnvironmentBlock();
    EXPECT_FALSE(table.empty());
    for (const auto& env : table) {
#if defined(OS_WIN)
        auto env_name = WideToASCII(env.first);
#else
        auto env_name = env.first;
#endif
        std::cout << env_name << "\n";
    }
}

}   // namespace kbase
