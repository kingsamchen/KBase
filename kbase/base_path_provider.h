/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_BASE_PATH_PROVIDER_H_
#define KBASE_BASE_PATH_PROVIDER_H_

#include "kbase/basic_types.h"

namespace kbase {

enum BasePathKey : PathKey {
    BASE_PATH_START = 0,
    FILE_EXE,               // path with filename of the current executable.
    FILE_MODULE,            // path with filename of the module running the code.
    DIR_CURRENT,            // current directory.
    DIR_EXE,                // directory containing FILE_EXE.
    DIR_MODULE,             // directory containing FILE_MODULE.
    DIR_TEMP,               // temporary directory.
    DIR_USER_DESKTOP,       // current user's desktop.
    DIR_PUBLIC_DESKTOP,     // public user's desktop.
    DIR_WINDOWS,            // windows directory, i.e. C:\Windows.
    DIR_SYSTEM,             // system directory, i.e. C:\Windows\System32.
    DIR_PROGRAM_FILES,      // C:\Program Files.
    DIR_PROGRAM_FILESX86,   // C:\Program Files or C:\Program Files (x86).
    DIR_APP_DATA,           // current user's application data directory.
    DIR_COMMON_APP_DATA,    // C:\ProgramData on or after system Vista.
    BASE_PATH_END
};

}   // namespace kbase

#endif // KBASE_BASE_PATH_PROVIDER_H_