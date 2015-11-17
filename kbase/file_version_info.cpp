/*
 @ Kingsley Chen
*/

#include "kbase\file_version_info.h"

#include <utility>

#include "kbase\error_exception_util.h"
#include "kbase\file_path.h"

#pragma comment(lib, "version.lib")

namespace {

using VersionData = kbase::internal::VersionData;

struct LangCodePage {
    WORD language;
    WORD code_page;
};

}   // namespace

namespace kbase {

// static
std::unique_ptr<FileVersionInfo> FileVersionInfo::CreateForFile(const FilePath& file)
{
    DWORD info_size = GetFileVersionInfoSizeW(file.value().c_str(), nullptr);
    ThrowLastErrorIf(info_size == 0, "failed to get file version info size.");

    VersionData version_data(info_size);
    BOOL rv = GetFileVersionInfoW(file.value().c_str(), 0, info_size,
                                  version_data.data());
    ThrowLastErrorIf(!rv, "failed to get file version info");

    return std::unique_ptr<FileVersionInfo>(
        new FileVersionInfo(std::move(version_data)));
}

// static
std::unique_ptr<FileVersionInfo> FileVersionInfo::CreateForModule(HMODULE module)
{
    wchar_t file_name[MAX_PATH];
    DWORD rv = GetModuleFileNameW(module, file_name, MAX_PATH);
    ThrowLastErrorIf(!rv, "failed to get file name for a module");

    return CreateForFile(kbase::FilePath(file_name));
}

FileVersionInfo::FileVersionInfo(VersionData&& data)
    : data_(std::move(data))
{
    // Set up VS_FIXEDFILEINFO struct.
    UINT dummy_size = 0U;
    VerQueryValueW(data_.data(), L"\\", reinterpret_cast<LPVOID*>(&info_block_),
                   &dummy_size);

    // Retrieve translation language and code page.
    LangCodePage* translation = nullptr;
    VerQueryValueW(data_.data(), L"\\VarFileInfo\\Translation",
                   reinterpret_cast<LPVOID*>(&translation), &dummy_size);
    lang_ = translation->language;
    code_page_ = translation->code_page;
}

std::wstring FileVersionInfo::GetValue(const wchar_t* name) const
{
    LangCodePage lang_codepage[2] {
        {lang_, code_page_}, {GetUserDefaultLangID(), code_page_}
    };

    std::wstring value;
    wchar_t sub_block[MAX_PATH];
    for (const auto& lang_cp : lang_codepage) {
        swprintf_s(sub_block, L"\\StringFileInfo\\%04x%04x\\%s", lang_cp.language,
                   lang_cp.code_page, name);
        LPVOID value_ptr = nullptr;
        UINT dummy_size = 0U;
        VerQueryValueW(data_.data(), sub_block, &value_ptr, &dummy_size);
        if (value_ptr) {
            value.assign(static_cast<const wchar_t*>(value_ptr));
            return value;
        }
    }

    return value;
}

}   // namespace kbase