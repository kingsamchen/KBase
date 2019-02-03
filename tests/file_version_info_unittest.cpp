/*
 @ 0xCCCCCCCC
*/

#include <iostream>

#include "catch2/catch.hpp"

#include "kbase/file_version_info.h"

namespace {

const kbase::Path kTargetFile(L"C:\\windows\\notepad.exe");

}   // namespace

namespace kbase {

TEST_CASE("Read version information for a file", "[FileVersionInfo]")
{
    SECTION("acquire FileVersionInfo struct")
    {
        auto version_info = kbase::FileVersionInfo::CreateForFile(kTargetFile);
        auto ffi = version_info->fixed_file_info();
        REQUIRE(ffi != nullptr);
        std::cout << ffi->dwFileVersionMS << ":" << ffi->dwFileVersionLS << std::endl;
    }

    SECTION("read values from FileVersionInfo")
    {
        // only test few values.
        auto version_info = kbase::FileVersionInfo::CreateForFile(kTargetFile);
        auto product_version = version_info->product_version();
        auto file_version = version_info->file_version();
        auto company = version_info->company_name();
        REQUIRE_FALSE(product_version.empty());
        REQUIRE_FALSE(file_version.empty());
        REQUIRE_FALSE(company.empty());
        std::wcout << product_version << std::endl << file_version << std::endl << company << std::endl;
    }
}

}   // namespace kbase
