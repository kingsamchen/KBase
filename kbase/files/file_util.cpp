
#include "kbase\files\file_util.h"

#include <Windows.h>

#include <algorithm>
#include <cstdlib>

#include "kbase\date_time.h"
#include "kbase\error_exception_util.h"

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
    // NOTE: both wcscpy_s and wcsncpy_s fill buffer after the first null-terminator
    // with dirty charater.
    wchar_t path_ends_double_null[MAX_PATH + 1] {0};
    std::copy_n(path.value().begin(), path.value().size(), path_ends_double_null);

    SHFILEOPSTRUCT file_op {0};
    file_op.fFlags = FOF_NO_UI;
    file_op.wFunc = FO_DELETE;
    file_op.pFrom = path_ends_double_null;

    int rv = SHFileOperationW(&file_op);
    bool err = rv || file_op.fAnyOperationsAborted;
    ThrowLastErrorIf(err, "Failed to remove files recursively");
}

}   // namespace kbase