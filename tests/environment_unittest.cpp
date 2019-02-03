/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include <iostream>

#include "kbase/environment.h"
#include "kbase/string_util.h"

namespace kbase {

TEST_CASE("Getting, setting or removing a variable from environment", "[Environment]")
{
    SECTION("get associated value for a given environment variable")
    {
        REQUIRE(Environment::GetVar(ENV_LITERAL("KCPath")).empty());
        REQUIRE_FALSE(Environment::GetVar(ENV_LITERAL("PATH")).empty());
    }

    SECTION("it is allowed to test if a variable exists")
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
            REQUIRE(p.second == Environment::HasVar(p.first.c_str()));
        }
    }

    SECTION("set up an environment variable")
    {
        const Environment::CharType* env_key = ENV_LITERAL("KCPath");
        Environment::StringType env_value = ENV_LITERAL("D:\\kc\\no1\\king");

        REQUIRE_FALSE(Environment::HasVar(env_key));
        Environment::SetVar(env_key, env_value);
        auto value = Environment::GetVar(env_key);
        REQUIRE(value == env_value);

        Environment::SetVar(env_key, Environment::StringType());
        REQUIRE(Environment::HasVar(env_key));
        value = Environment::GetVar(env_key);
        REQUIRE(value.empty());
    }

    SECTION("remove a given variable and no effect if no such variable")
    {
        const Environment::CharType* env_key = ENV_LITERAL("KCPath");

        REQUIRE(Environment::HasVar(env_key));
        Environment::RemoveVar(env_key);
        REQUIRE_FALSE(Environment::HasVar(env_key));

        // Do no harm.
        Environment::RemoveVar(ENV_LITERAL("non-sense"));
    }
}

TEST_CASE("Obtain current environment variable list", "[Environment]")
{
    auto table = Environment::CurrentEnvironmentBlock();
    REQUIRE_FALSE(table.empty());

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
