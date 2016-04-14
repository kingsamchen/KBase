/*
 @ 0xCCCCCCCC
*/

#include "kbase/base_path_provider.h"

#include <Windows.h>
#include <ShlObj.h>

#include "kbase/error_exception_util.h"
#include "kbase/path_service.h"
#include "kbase/scope_guard.h"

// See @ http://blogs.msdn.com/b/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

using kbase::Path;

Path ShellGetFolderPath(const KNOWNFOLDERID& folder_id)
{
    wchar_t* folder_path = nullptr;
    HRESULT ret = SHGetKnownFolderPath(folder_id, 0, nullptr, &folder_path);
    ON_SCOPE_EXIT([&] { CoTaskMemFree(folder_path); });
    ENSURE(CHECK, ret == S_OK)(ret).Require();

    return Path(folder_path);
}

}   // namespace

namespace kbase {

Path BasePathProvider(PathKey key)
{
    // Though the system does have support for long file path, I decide to ignore
    // it here.
    const size_t kMaxPath = MAX_PATH + 1;
    wchar_t buffer[kMaxPath] {0};
    Path path;

    switch (key) {
        case FILE_EXE:
            GetModuleFileName(nullptr, buffer, kMaxPath);
            path = Path(buffer);
            break;

        case FILE_MODULE: {
            HMODULE module = reinterpret_cast<HMODULE>(&__ImageBase);
            GetModuleFileName(module, buffer, kMaxPath);
            path = Path(buffer);
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
            path = Path(buffer);
            break;

        case DIR_TEMP:
            GetTempPath(kMaxPath, buffer);
            path = Path(buffer);
            break;

        case DIR_USER_DESKTOP:
            path = ShellGetFolderPath(FOLDERID_Desktop);
            break;

        case DIR_PUBLIC_DESKTOP:
            path = ShellGetFolderPath(FOLDERID_PublicDesktop);
            break;

        case DIR_WINDOWS:
            GetWindowsDirectory(buffer, kMaxPath);
            path = Path(buffer);
            break;

        case DIR_SYSTEM:
            GetSystemDirectory(buffer, kMaxPath);
            path = Path(buffer);
            break;

        case DIR_PROGRAM_FILES:
            path = ShellGetFolderPath(FOLDERID_ProgramFiles);
            break;

        case DIR_PROGRAM_FILESX86:
            path = ShellGetFolderPath(FOLDERID_ProgramFilesX86);
            break;

        case DIR_APP_DATA:
            path = ShellGetFolderPath(FOLDERID_RoamingAppData);
            break;

        case DIR_COMMON_APP_DATA:
            path = ShellGetFolderPath(FOLDERID_ProgramData);
            break;

        default:
            break;
    }

    return path;
}

}   // namespace kbase