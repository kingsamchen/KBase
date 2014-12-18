
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILES_FILE_VERSION_INFO_H_
#define KBASE_FILES_FILE_VERSION_INFO_H_

#include <Windows.h>

#include <memory>
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

    static std::unique_ptr<FileVersionInfo> CreateForFile(const FilePath& file_path);

    static std::unique_ptr<FileVersionInfo> CreateForModule(const HMODULE module);

    // TODO: property fields.

private:
    FileVersionInfo(internal::VersionData&& data);
    
private:
    internal::VersionData data_;
    VS_FIXEDFILEINFO* info_block_;
    unsigned short lang_;
    unsigned short code_page_;
};

}   // namespace kbase

#endif  // KBASE_FILES_FILE_VERSION_INFO_H_