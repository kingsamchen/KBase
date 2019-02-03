/*
 @ 0xCCCCCCCC
*/

#include <utility>

#include "catch2/catch.hpp"

#include "kbase/registry.h"
#include "kbase/string_encoding_conversions.h"

namespace {

using RegKeyItem = std::pair<HKEY, const wchar_t*>;

RegKeyItem key_items[] {
    {HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\.NETFramework)"},   // in WOW6432Node
    {HKEY_LOCAL_MACHINE, LR"(SOFTWARE\7-Zip)"},
    {HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet)"}
};

}   // namespace

namespace kbase {

TEST_CASE("Basic usages", "[Registry]")
{
    const wchar_t subkey[] = L"SOFTWARE\\KBase";

    DWORD disposition = 0;
    RegKey::Create(HKEY_CURRENT_USER, subkey, KEY_CREATE_SUB_KEY, disposition);
    REQUIRE(REG_CREATED_NEW_KEY == disposition);

    REQUIRE(RegKey::KeyExists(HKEY_CURRENT_USER, subkey));

    RegKey key;
    key.Open(HKEY_CURRENT_USER, L"SOFTWARE",
             DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_SET_VALUE);

    key.DeleteKey(L"KBase");
    REQUIRE_FALSE(RegKey::KeyExists(HKEY_CURRENT_USER, subkey));
}

TEST_CASE("Open or create registry keys", "[Registry]")
{
    SECTION("open keys")
    {
        RegKey key;
        REQUIRE_FALSE(static_cast<bool>(key));
        REQUIRE(key.subkey_name().empty());

        key.Open(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet)", KEY_READ);
        REQUIRE(static_cast<bool>(key));
        REQUIRE_FALSE(key.subkey_name().empty());

        key.Open(L"Control", KEY_READ);
        REQUIRE(static_cast<bool>(key));

        key.Open(L"FakeSubkey", KEY_READ);
        REQUIRE_FALSE(static_cast<bool>(key));
        REQUIRE(key.subkey_name().empty());
    }

    SECTION("create keys")
    {
        RegKey key(RegKey::Create(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet)", KEY_READ));
        REQUIRE(static_cast<bool>(key));

        DWORD disposition = 0;
        RegKey::Create(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet)", KEY_READ, disposition);
        REQUIRE(disposition == REG_OPENED_EXISTING_KEY);
    }
}

TEST_CASE("RegKey instance is movable", "[Registry]")
{
    RegKey key(RegKey::Create(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet)", KEY_READ));
    REQUIRE(static_cast<bool>(key));

    RegKey another_key;
    REQUIRE_FALSE(static_cast<bool>(another_key));
    HKEY raw_handle_value = key.Get();
    another_key = std::move(key);
    REQUIRE_FALSE(static_cast<bool>(key));
    REQUIRE(raw_handle_value == another_key.Get());
}

TEST_CASE("Check if a key exists", "[Registry]")
{
    using ExistTestItem = std::tuple<RegKeyItem, kbase::RegKey::WOW6432Node, bool>;
    ExistTestItem case_items[] {
        std::make_tuple(key_items[0], kbase::RegKey::DefaultKeyOnWOW64, true),
        std::make_tuple(key_items[1], kbase::RegKey::Force64KeyOnWOW64, true),
#if !defined(_M_X64)
        std::make_tuple(key_items[1], kbase::RegKey::DefaultKeyOnWOW64, false),
#else
        std::make_tuple(key_items[1], kbase::RegKey::DefaultKeyOnWOW64, true),
#endif
        std::make_tuple(key_items[2], kbase::RegKey::DefaultKeyOnWOW64, true),
        std::make_tuple(RegKeyItem(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet1)"),
                        kbase::RegKey::DefaultKeyOnWOW64, false)
    };

    for (const auto& item : case_items) {
        auto key = std::get<0>(item);
        bool rv = kbase::RegKey::KeyExists(key.first, key.second, std::get<1>(item));
        INFO("for " << std::get<0>(item).second << "\n");
        REQUIRE(rv == std::get<2>(item));
    }
}

TEST_CASE("Operations on values", "[Registry]")
{
    auto key = RegKey::Create(HKEY_CURRENT_USER, L"SOFTWARE\\KBase", KEY_READ | KEY_WRITE);
    REQUIRE(static_cast<bool>(key));

    REQUIRE_FALSE(key.HasValue(L"Alpha"));
    REQUIRE(0 == key.GetValueCount());

    {
        key.WriteValue(L"Alpha", static_cast<DWORD>(0xDEADBEEF));
        REQUIRE(key.HasValue(L"Alpha"));
        key.WriteValue(L"Bravo", static_cast<DWORD64>(0xDEADBEEF));
        std::wstring content(L"Mayday!");
        key.WriteValue(L"Charlie", content.data(), content.length());
        REQUIRE(3 == key.GetValueCount());
    }

    {
        std::wstring value_name;

        key.GetValueNameAt(0, value_name);
        REQUIRE(std::wstring(L"Alpha") == value_name);
        DWORD dval;
        key.ReadValue(value_name.c_str(), dval);
        REQUIRE(static_cast<DWORD>(0xDEADBEEF) == dval);

        key.GetValueNameAt(1, value_name);
        REQUIRE(std::wstring(L"Bravo") == value_name);
        DWORD64 qval;
        key.ReadValue(value_name.c_str(), qval);
        REQUIRE(static_cast<DWORD>(0xDEADBEEF) == qval);

        key.GetValueNameAt(2, value_name);
        REQUIRE(std::wstring(L"Charlie") == value_name);
        std::wstring sval;
        key.ReadValue(value_name.c_str(), sval);
        REQUIRE(std::wstring(L"Mayday!") == sval);
    }

    RegKey dk;
    dk.Open(HKEY_CURRENT_USER, L"SOFTWARE", DELETE | KEY_READ | KEY_WRITE);
    dk.DeleteKey(L"KBase");
}

TEST_CASE("We can iterate registry keys as using iterators", "[Registry]")
{
    SECTION("basic")
    {
        RegKeyIterator regkey_it(HKEY_CURRENT_USER, L"SOFTWARE");
        REQUIRE(regkey_it != RegKeyIterator());

        auto name = regkey_it->subkey_name();
        REQUIRE_FALSE(name.empty());
        REQUIRE(static_cast<bool>(*regkey_it));

        {
            RegKeyIterator copy(regkey_it);
            REQUIRE(copy == regkey_it);
            REQUIRE(copy->subkey_name() == regkey_it->subkey_name());
        }

        REQUIRE(static_cast<bool>(*regkey_it));
    }

    SECTION("iteration")
    {
        RegKeyIterator it(HKEY_CURRENT_USER, L"SOFTWARE");
        while (it != RegKeyIterator()) {
            ++it;
        }

        REQUIRE(it == RegKeyIterator());
    }

    SECTION("in range-based for")
    {
        RegKeyIterator it(HKEY_CURRENT_USER, L"SOFTWARE");
        bool rv = false;
        for (const auto& key : it) {
            rv |= static_cast<bool>(key);
            INFO(WideToUTF8(key.subkey_name()) << "\n");
        }

        REQUIRE(rv);
    }
}

}   // namespace kbase
