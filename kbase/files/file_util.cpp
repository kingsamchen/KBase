
#include "kbase\files\file_util.h"

#include <Windows.h>

#include <algorithm>
#include <cstdlib>

#include "kbase\date_time.h"
#include "kbase\error_exception_util.h"
#include "kbase\files\file_enumerator.h"
#include "kbase\strings\string_util.h"

namespace kbase {

FilePath MakeAbsoluteFilePath(const FilePath& path)
{
    wchar_t buffer[_MAX_PATH];
    if (!_wfullpath(buffer, path.value().c_str(), _MAX_PATH)) {
        return FilePath();
    }

    return FilePath(buffer);
}

bool PathExists(const FilePath& path)
{
    return GetFileAttributesW(path.value().c_str()) != INVALID_FILE_ATTRIBUTES;
}

bool DirectoryExists(const FilePath& path)
{
    DWORD attr = GetFileAttributesW(path.value().c_str());
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool IsDirectoryEmpty(const FilePath& path)
{
    FileEnumerator file_it(path, false, FileEnumerator::DIRS | FileEnumerator::FILES);
    return file_it.Next().empty();
}

FileInfo GetFileInfo(const FilePath& path)
{
    WIN32_FILE_ATTRIBUTE_DATA attr_data;
    BOOL ret = GetFileAttributesExW(path.value().c_str(), GetFileExInfoStandard,
                                    &attr_data);
    ThrowLastErrorIf(!ret, "Failed to call GetFileAttributesEx");

    ULARGE_INTEGER file_size;
    file_size.HighPart = attr_data.nFileSizeHigh;
    file_size.LowPart = attr_data.nFileSizeLow;

    return FileInfo(path.BaseName().value(),
                    static_cast<int64_t>(file_size.QuadPart),
                    !!(attr_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY),
                    DateTime(attr_data.ftCreationTime),
                    DateTime(attr_data.ftLastWriteTime),
                    DateTime(attr_data.ftLastAccessTime));
}

void RemoveFile(const FilePath& path, bool recursive)
{
    if (!recursive) {
        if (GetFileInfo(path).is_directory()) {
            BOOL rv = RemoveDirectoryW(path.value().c_str());
            ThrowLastErrorIf(!rv, "Failed to remove the directory");
            return;
        }

        BOOL rv = DeleteFileW(path.value().c_str());
        ThrowLastErrorIf(!rv, "Failed to delete the file");
        return;
    }
    
    // SHFileOperationW requires that path must end with double null-terminators.
    // Moreover, if the path passed to SHFileOperationW is not a full path, the
    // invocation of SHFileOperationW is not thread safe.
    // NOTE: both wcscpy_s and wcsncpy_s fill buffer after the first null-terminator
    // with dirty charater.
    wchar_t path_ends_double_null[MAX_PATH + 1] {0};
    FilePath full_path = MakeAbsoluteFilePath(path);
    std::copy_n(full_path.value().begin(), full_path.value().size(),
                path_ends_double_null);

    SHFILEOPSTRUCT file_op {0};
    file_op.fFlags = FOF_NO_UI;
    file_op.wFunc = FO_DELETE;
    file_op.pFrom = path_ends_double_null;

    int rv = SHFileOperationW(&file_op);
    bool err = rv || file_op.fAnyOperationsAborted;
    ThrowLastErrorIf(err, "Failed to remove files recursively");
}

void RemoveFileAfterReboot(const FilePath& path)
{
    BOOL rv = MoveFileExW(path.value().c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
    ThrowLastErrorIf(!rv, "Failed to mark delay delete");
}

void DuplicateFile(const FilePath& src, const FilePath& dest)
{
    BOOL rv = CopyFileW(src.value().c_str(), dest.value().c_str(), false);
    ThrowLastErrorIf(!rv, "Failed to duplicate file");
}

void DuplicateDirectory(const FilePath& src, const FilePath& dest, bool recursive)
{
    FilePath full_src = MakeAbsoluteFilePath(src);
    ENSURE(!full_src.empty())(src.value()).raise();
    FilePath full_dest = dest;
    if (PathExists(full_dest)) {
        full_dest = MakeAbsoluteFilePath(full_dest);
        ENSURE(!full_dest.empty())(dest.value()).raise();
    } else {
        // Parent directory of the |dest| must exist.
        auto&& dest_parent = MakeAbsoluteFilePath(full_dest.DirName());
        ENSURE(!dest_parent.empty())(dest.value()).raise();
    }

    // Treats this condition as succeeded.
    if (full_src == full_dest) {
        return;
    }

    // The destination cannot be a subfolder of the source in recursive mode.
    bool permitted = !(recursive && 
                       StartsWith(full_dest.value(), full_src.value(), false));
    ENSURE(permitted)(full_src.value())(full_dest.value()).raise();

    if (!DirectoryExists(full_dest)) {
        BOOL rv = CreateDirectoryW(full_dest.value().c_str(), nullptr);
        ThrowLastErrorIf(!rv, "Failed to create top-level dest dir");
    }

    int file_type = FileEnumerator::FILES;
    if (recursive) {
        file_type |= FileEnumerator::DIRS;
    }

    FileEnumerator file_it(full_src, recursive, file_type);
    for (auto&& cur = file_it.Next(); !cur.empty(); cur = file_it.Next()) {
        FilePath dest_for_cur = full_dest;
        bool rv = full_src.AppendRelativePath(cur, &dest_for_cur);
        ENSURE(rv)(full_src.value())(cur.value())(dest_for_cur.value()).raise();
        if (file_it.GetInfo().is_directory() && !DirectoryExists(dest_for_cur)) {
            BOOL rv = CreateDirectoryW(dest_for_cur.value().c_str(), nullptr);
            ThrowLastErrorIf(!rv, "Failed to create top-level dest dir");
        } else {
            DuplicateFile(cur, dest_for_cur);
        }
    }
}

void MakeFileMove(const FilePath& src, const FilePath& dest)
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

    SetLastError(last_error.last_error_code());
    ThrowLastErrorIf(true, "Failed to move file");
}

}   // namespace kbase