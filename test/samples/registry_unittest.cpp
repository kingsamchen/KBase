/*
 @ 0xCCCCCCCC
*/

#include <utility>

#include "gtest/gtest.h"

#include "kbase/registry.h"

namespace {

using RegKeyItem = std::pair<HKEY, const wchar_t*>;

RegKeyItem key_items[] {
    {HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\.NETFramework)"},   // in WOW6432Node
    {HKEY_LOCAL_MACHINE, LR"(SOFTWARE\7-Zip)"},
    {HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet)"}
};

}   // namespace

namespace kbase {

TEST(RegistryTest, CreateCheckAndDeleteKey)
{
    const wchar_t subkey[] = L"SOFTWARE\\KBase";

    DWORD disposition = 0;
    RegKey::Create(HKEY_CURRENT_USER, subkey, KEY_CREATE_SUB_KEY, disposition);
    EXPECT_EQ(REG_CREATED_NEW_KEY, disposition);

    EXPECT_TRUE(RegKey::KeyExists(HKEY_CURRENT_USER, subkey));

    RegKey key;
    key.Open(HKEY_CURRENT_USER, L"SOFTWARE",
             DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_SET_VALUE);

    key.DeleteKey(L"KBase");
    EXPECT_FALSE(RegKey::KeyExists(HKEY_CURRENT_USER, subkey));
}

TEST(RegistryTest, OpenKeys)
{
    {
        RegKey key;
        EXPECT_FALSE(static_cast<bool>(key));
        EXPECT_TRUE(key.subkey_name().empty());

        key.Open(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet)", KEY_READ);
        EXPECT_TRUE(static_cast<bool>(key));
        EXPECT_FALSE(key.subkey_name().empty());

        key.Open(L"Control", KEY_READ);
        EXPECT_TRUE(static_cast<bool>(key));

        key.Open(L"FakeSubkey", KEY_READ);
        EXPECT_FALSE(static_cast<bool>(key));
        EXPECT_TRUE(key.subkey_name().empty());
    }

    {
        RegKey key(RegKey::Create(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet)", KEY_READ));
        EXPECT_TRUE(static_cast<bool>(key));

        DWORD disposition = 0;
        RegKey::Create(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet)", KEY_READ, disposition);
        EXPECT_EQ(disposition, REG_OPENED_EXISTING_KEY);
    }
}

TEST(RegistryTest, MoveSemantics)
{
    RegKey key(RegKey::Create(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet)", KEY_READ));
    ASSERT_TRUE(static_cast<bool>(key));

    RegKey another_key;
    EXPECT_FALSE(static_cast<bool>(another_key));
    HKEY raw_handle_value = key.Get();
    another_key = std::move(key);
    EXPECT_FALSE(static_cast<bool>(key));
    EXPECT_EQ(raw_handle_value, another_key.Get());
}

TEST(RegistryTest, KeyExists)
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
        std::wcout << L"for " << std::get<0>(item).second << std::endl;
        EXPECT_EQ(rv, std::get<2>(item));
    }
}

TEST(RegistryTest, ValueOperations)
{
    auto key = RegKey::Create(HKEY_CURRENT_USER, L"SOFTWARE\\KBase", KEY_READ | KEY_WRITE);
    ASSERT_TRUE(static_cast<bool>(key));

    EXPECT_FALSE(key.HasValue(L"Alpha"));
    EXPECT_EQ(0, key.GetValueCount());

    key.WriteValue(L"Alpha", static_cast<DWORD>(0xDEADBEEF));
    EXPECT_TRUE(key.HasValue(L"Alpha"));
    key.WriteValue(L"Bravo", static_cast<DWORD64>(0xDEADBEEF));
    std::wstring content(L"Mayday!");
    key.WriteValue(L"Charlie", content.data(), content.length());
    EXPECT_EQ(3, key.GetValueCount());

    std::wstring value_name;

    key.GetValueNameAt(0, value_name);
    EXPECT_EQ(std::wstring(L"Alpha"), value_name);
    DWORD dval;
    key.ReadValue(value_name.c_str(), dval);
    EXPECT_EQ(static_cast<DWORD>(0xDEADBEEF), dval);

    key.GetValueNameAt(1, value_name);
    EXPECT_EQ(std::wstring(L"Bravo"), value_name);
    DWORD64 qval;
    key.ReadValue(value_name.c_str(), qval);
    EXPECT_EQ(static_cast<DWORD>(0xDEADBEEF), qval);

    key.GetValueNameAt(2, value_name);
    EXPECT_EQ(std::wstring(L"Charlie"), value_name);
    std::wstring sval;
    key.ReadValue(value_name.c_str(), sval);
    EXPECT_EQ(std::wstring(L"Mayday!"), sval);

    RegKey dk;
    dk.Open(HKEY_CURRENT_USER, L"SOFTWARE", DELETE | KEY_READ | KEY_WRITE);
    dk.DeleteKey(L"KBase");
}

TEST(RegistryTest, RegKeyIteratorBasic)
{
    RegKeyIterator regkey_it(HKEY_CURRENT_USER, L"SOFTWARE");
    ASSERT_NE(regkey_it, RegKeyIterator());

    auto name = regkey_it->subkey_name();
    EXPECT_FALSE(name.empty());
    EXPECT_TRUE(static_cast<bool>(*regkey_it));

    {
        RegKeyIterator copy(regkey_it);
        EXPECT_EQ(copy ,regkey_it);
        EXPECT_EQ(copy->subkey_name(), regkey_it->subkey_name());
    }

    EXPECT_TRUE(static_cast<bool>(*regkey_it));
}

TEST(RegistryTest, RegKeyIteratorIteration)
{
    RegKeyIterator it(HKEY_CURRENT_USER, L"SOFTWARE");
    while (it != RegKeyIterator()) {
        ++it;
    }

    EXPECT_EQ(it, RegKeyIterator());
}

TEST(RegistryTest, RegKeyIteratorRangeBasedFor)
{
    RegKeyIterator it(HKEY_CURRENT_USER, L"SOFTWARE");
    bool rv = false;
    for (const auto& key : it) {
        rv |= static_cast<bool>(key);
        std::wcout << key.subkey_name() << "\n";
    }

    EXPECT_TRUE(rv);
}

}   // namespace kbase
