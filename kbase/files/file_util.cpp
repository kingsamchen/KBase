
#include "kbase\files\file_util.h"

#include <Windows.h>

#include <cstdlib>

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
    return GetFileAttributes(path.value().c_str()) != INVALID_FILE_ATTRIBUTES;
}

}   // namespace kbase