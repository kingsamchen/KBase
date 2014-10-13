
#include "kbase\base_path_provider.h"

#include <Windows.h>

#include "kbase\path_service.h"

// See @ http://blogs.msdn.com/b/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

}   // namespace

namespace kbase {

FilePath BasePathProvider(PathKey key)
{
    // Though the system does have support for long file path, I decide to ignore 
    // it here.
    const size_t kMaxPath = MAX_PATH + 1;
    wchar_t buffer[kMaxPath] {0};
    FilePath path;

    switch (key) {
        case FILE_EXE:
          GetModuleFileName(nullptr, buffer, kMaxPath);
            path = FilePath(buffer);
            break;

        case FILE_MODULE: {
            HMODULE module = reinterpret_cast<HMODULE>(&__ImageBase);
            GetModuleFileName(module, buffer, kMaxPath);
            path = FilePath(buffer);
            break;
        }

        case DIR_EXE:
            path = PathService::Get(FILE_EXE).DirName();
            break;

        case DIR_MODULE:
            path = PathService::Get(FILE_MODULE).DirName();
            break;

        case DIR_CURRENT:
            GetCurrentDirectory(kMaxPath, buffer);
            path = FilePath(buffer);
            break;

        case DIR_TEMP:
            GetTempPath(kMaxPath, buffer);
            path = FilePath(buffer);
            break;

        default:
            break;
    }

    return path;
}

}   // namespace kbase