
#include "kbase\files\file_util.h"

#include <Windows.h>

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

}   // namespace kbase