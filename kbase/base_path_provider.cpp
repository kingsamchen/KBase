
#include "kbase\base_path_provider.h"

#include <Windows.h>

#include "kbase\files\file_path.h"

namespace {

}   // namespace

namespace kbase {

FilePath BasePathProvider(PathKey key)
{
    // Though the system does have support for long file path, I decide to ignore 
    // it here.
    wchar_t buffer[MAX_PATH] {0};
    FilePath path;

    switch (key) {
        case FILE_EXE:
            GetModuleFileName(nullptr, buffer, MAX_PATH);
            path = FilePath(buffer);
            break;

        case FILE_MODULE:
            break;

        default:
            break;
    }

    return path;
}

}   // namespace kbase