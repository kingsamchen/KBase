/*
 @ 0xCCCCCCCC
*/

#include "kbase/file_util.h"

#include <Windows.h>

#include <algorithm>
#include <fstream>

#include "kbase/chrono_util.h"
#include "kbase/error_exception_util.h"
#include "kbase/file_iterator.h"
#include "kbase/logging.h"
#include "kbase/string_util.h"

namespace kbase {

Path MakeAbsoluteFilePath(const Path& path)
{
    wchar_t buffer[_MAX_PATH];
    if (!_wfullpath(buffer, path.value().c_str(), _MAX_PATH)) {
        return Path();
    }

    return Path(buffer);
}

bool PathExists(const Path& path)
{
    return GetFileAttributesW(path.value().c_str()) != INVALID_FILE_ATTRIBUTES;
}

bool DirectoryExists(const Path& path)
{
    DWORD attr = GetFileAttributesW(path.value().c_str());
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool IsDirectoryEmpty(const Path& path)
{
    return FileIterator(path, false) == FileIterator();;
}

FileInfo GetFileInfo(const Path& path)
{
    WIN32_FILE_ATTRIBUTE_DATA attr_data;
    BOOL ret = GetFileAttributesExW(path.value().c_str(), GetFileExInfoStandard,
                                    &attr_data);
    ENSURE(THROW, ret != 0)(LastError()).Require("Failed to call GetFileAttributesEx");

    ULARGE_INTEGER file_size;
    file_size.HighPart = attr_data.nFileSizeHigh;
    file_size.LowPart = attr_data.nFileSizeLow;

    return FileInfo(path,
                    static_cast<int64_t>(file_size.QuadPart),
                    !!(attr_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY),
                    FileTime(attr_data.ftCreationTime),
                    FileTime(attr_data.ftLastWriteTime),
                    FileTime(attr_data.ftLastAccessTime));
}

void RemoveFile(const Path& path, bool recursive)
{
    if (!recursive) {
        if (GetFileInfo(path).is_directory()) {
            BOOL rv = RemoveDirectoryW(path.value().c_str());
            ENSURE(THROW, rv != 0)(LastError()).Require("Failed to remove the directory");
            return;
        }

        BOOL rv = DeleteFileW(path.value().c_str());
        ENSURE(THROW, rv != 0)(LastError()).Require("Failed to delete the file");
        return;
    }

    // SHFileOperationW requires that path must end with double null-terminators.
    // Moreover, if the path passed to SHFileOperationW is not a full path, the
    // invocation of SHFileOperationW is not thread safe.
    // NOTE: both wcscpy_s and wcsncpy_s fill buffer after the first null-terminator
    // with dirty charater.
    wchar_t path_ends_double_null[MAX_PATH + 1] {0};
    Path full_path = MakeAbsoluteFilePath(path);
    std::copy_n(full_path.value().begin(), full_path.value().size(),
                path_ends_double_null);

    SHFILEOPSTRUCT file_op {0};
    file_op.fFlags = FOF_NO_UI;
    file_op.wFunc = FO_DELETE;
    file_op.pFrom = path_ends_double_null;

    int rv = SHFileOperationW(&file_op);
    bool err = rv || file_op.fAnyOperationsAborted;
    ENSURE(THROW, !err)(LastError()).Require("Failed to remove files recursively");
}

void RemoveFileAfterReboot(const Path& path)
{
    BOOL rv = MoveFileExW(path.value().c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
    ENSURE(THROW, rv != 0)(LastError()).Require("Failed to mark delay delete");
}

void DuplicateFile(const Path& src, const Path& dest)
{
    BOOL rv = CopyFileW(src.value().c_str(), dest.value().c_str(), false);
    ENSURE(THROW, rv != 0)(LastError()).Require("Failed to duplicate file");
}

void DuplicateDirectory(const Path& src, const Path& dest, bool recursive)
{
    Path full_src = MakeAbsoluteFilePath(src);
    ENSURE(CHECK, !full_src.empty())(src.value()).Require();
    Path full_dest = dest;
    if (PathExists(full_dest)) {
        full_dest = MakeAbsoluteFilePath(full_dest);
        ENSURE(CHECK, !full_dest.empty())(dest.value()).Require();
    } else {
        // Parent directory of the `dest` must exist.
        auto&& dest_parent = MakeAbsoluteFilePath(full_dest.parent_path());
        ENSURE(CHECK, !dest_parent.empty())(dest.value()).Require();
    }

    // Treats this condition as succeeded.
    if (full_src == full_dest) {
        return;
    }

    // The destination cannot be a subfolder of the source in recursive mode.
    bool permitted = !(recursive &&
                       StartsWith(full_dest.value(), full_src.value(), CaseMode::ASCIIInsensitive));
    ENSURE(CHECK, permitted)(full_src.value())(full_dest.value()).Require();

    if (!DirectoryExists(full_dest)) {
        BOOL rv = CreateDirectoryW(full_dest.value().c_str(), nullptr);
        ENSURE(THROW, rv != 0)(LastError()).Require("Failed to create top-level dest dir");
    }

    for (FileIterator fit(full_src, recursive); fit != FileIterator(); ++fit) {
        Path dest_for_cur = full_dest;
        bool rv = full_src.AppendRelativePath(fit->file_path(), &dest_for_cur);
        ENSURE(CHECK, rv)(full_src.value())(fit->file_path().value())(dest_for_cur.value()).Require();
        if (fit->is_directory() && !DirectoryExists(dest_for_cur)) {
            BOOL ret = CreateDirectoryW(dest_for_cur.value().c_str(), nullptr);
            ENSURE(THROW, ret != 0)(LastError()).Require("Failed to create top-level dest dir");
        } else {
            DuplicateFile(fit->file_path(), dest_for_cur);
        }
    }
}

void MakeFileMove(const Path& src, const Path& dest)
{
    if (MoveFileExW(src.value().c_str(), dest.value().c_str(),
                    MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
        return;
    }

    LastError last_error;

    if (DirectoryExists(src)) {
        // Only empty directory can be moved across volume.
        // We here use an explicit copy-and-remove strategy.
        DuplicateDirectory(src, dest, true);
        RemoveFile(src, true);
        return;
    }

    SetLastError(last_error.error_code());
    ENSURE(THROW, NotReached())(LastError()).Require("Failed to move file");
}

void ReadFileToString(const Path& path, std::string& data)
{
    if (!data.empty()) {
        data.clear();
    }

    // It seems the constructor of ifstream in MSVC has an overload function for
    // wide-character.
    std::ifstream in(path.value(), std::ios::in | std::ios::binary);
    if (!in) {
        DLOG(WARNING) << "Create/open file failed for path " << path.AsUTF8();
        return;
    }

    in.seekg(0, std::ios::end);
    data.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0);
    in.read(&data[0], data.size());
}

std::string ReadFileToString(const Path& path)
{
    std::string data;
    ReadFileToString(path, data);

    return data;
}

void WriteStringToFile(const Path& path, const std::string& data)
{
    std::ofstream out(path.value());
    if (!out) {
        DLOG(WARNING) << "Create/open file faield for path " << path.AsUTF8();
        return;
    }

    out.write(data.data(), data.size());
}

void AppendStringToFile(const Path& path, const std::string& data)
{
    std::ofstream out(path.value(), std::ios::app);
    if (!out) {
        DLOG(WARNING) << "Create/open file faield for path " << path.AsUTF8();
        return;
    }

    out.write(data.data(), data.size());
}

}   // namespace kbase