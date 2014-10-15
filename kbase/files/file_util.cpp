
#include "kbase\files\file_util.h"

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

}   // namespace kbase