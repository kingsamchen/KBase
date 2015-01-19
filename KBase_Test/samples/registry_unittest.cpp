/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\registry.h"

#include <utility>

namespace {

using RegKeyItem = std::pair<HKEY, const wchar_t*>;

RegKeyItem key_items[] {
    {HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\.NETFramework)"},   // in WOW6432Node
    {HKEY_LOCAL_MACHINE, LR"(SOFTWARE\7-Zip)"},
    {HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet)"}
};

}   // namespace

TEST(RegistryTest, KeyExists)
{
    using ExistTestItem = std::tuple<RegKeyItem, kbase::RegKey::WOW6432Node, bool>;
    ExistTestItem case_items[] {
        std::make_tuple(key_items[0], kbase::RegKey::DEFAULT_WOW64_KEY, true),
        std::make_tuple(key_items[1], kbase::RegKey::FORCE_WOW64_64KEY, true),
#if !defined(_M_X64)
        std::make_tuple(key_items[1], kbase::RegKey::DEFAULT_WOW64_KEY, false),
#else
        std::make_tuple(key_items[1], kbase::RegKey::DEFAULT_WOW64_KEY, true),
#endif
        std::make_tuple(key_items[2], kbase::RegKey::DEFAULT_WOW64_KEY, true),
        std::make_tuple(RegKeyItem(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet1)"),
                        kbase::RegKey::DEFAULT_WOW64_KEY, false)
    };

    for (const auto& item : case_items) {
        auto key = std::get<0>(item);
        bool rv = kbase::RegKey::KeyExists(key.first, key.second, std::get<1>(item));
        std::wcout << L"for " << std::get<0>(item).second << std::endl;
        EXPECT_EQ(rv, std::get<2>(item));
    }
}