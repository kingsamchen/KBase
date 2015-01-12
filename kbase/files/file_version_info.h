/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILES_FILE_VERSION_INFO_H_
#define KBASE_FILES_FILE_VERSION_INFO_H_

#include <Windows.h>

#include <memory>
#include <string>
#include <vector>

#include "kbase\basic_types.h"

namespace kbase {

namespace internal {

using VersionData = std::vector<kbase::byte>;

}   // namespace internal

class FilePath;

class FileVersionInfo {
public:
    ~FileVersionInfo() = default;

    FileVersionInfo(const FileVersionInfo&) = delete;

    FileVersionInfo(FileVersionInfo&&) = delete;

    FileVersionInfo& operator=(const FileVersionInfo&) = delete;

    FileVersionInfo& operator=(FileVersionInfo&&) = delete;

    static std::unique_ptr<FileVersionInfo> CreateForFile(const FilePath& file);

    static std::unique_ptr<FileVersionInfo> CreateForModule(HMODULE module);

    VS_FIXEDFILEINFO* fixed_file_info() const
    {
        return info_block_;
    }

    std::wstring comments() const
    {
        return GetValue(L"Comments");
    }

    std::wstring internal_name() const
    {
        return GetValue(L"InternalName");
    }

    std::wstring product_name() const
    {
        return GetValue(L"ProductName");
    }

    std::wstring company_name() const
    {
        return GetValue(L"CompanyName");
    }

    std::wstring legal_copyright() const
    {
        return GetValue(L"LegalCopyright");
    }

    std::wstring product_version() const
    {
        return GetValue(L"ProductVersion");
    }

    std::wstring file_description() const
    {
        return GetValue(L"FileDescription");
    }

    std::wstring legal_trademarks() const
    {
        return GetValue(L"LegalTrademarks");
    }

    std::wstring private_build() const
    {
        return GetValue(L"PrivateBuild");
    }

    std::wstring file_version() const
    {
        return GetValue(L"FileVersion");
    }

    std::wstring original_filename() const
    {
        return GetValue(L"OriginalFilename");
    }

    std::wstring special_build() const
    {
        return GetValue(L"SpecialBuild");
    }

private:
    FileVersionInfo(internal::VersionData&& data);

    // Returns the value associated with the |name|.
    // Returns an empty string otherwise.
    std::wstring GetValue(const wchar_t* name) const;

private:
    internal::VersionData data_;
    VS_FIXEDFILEINFO* info_block_ = nullptr;
    unsigned short lang_ = 0U;
    unsigned short code_page_ = 0U;
};

}   // namespace kbase

#endif  // KBASE_FILES_FILE_VERSION_INFO_H_