
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_BASE_PATH_PROVIDER_H_
#define KBASE_BASE_PATH_PROVIDER_H_

#include "kbase\basic_types.h"

namespace kbase {

enum BasePathKey : PathKey {
    BASE_PATH_START = 0,
    FILE_EXE,               // path with filename of the current executable.
    FILE_MODULE,            // path with filename of the module running the code.
    DIR_CURRENT,            // current directory.
    DIR_EXE,                // directory containing FILE_EXE.
    DIR_MODULE,             // directory containing FILE_MODULE.
    DIR_TEMP,               // temporary directory.
    DIR_USER_DESKTOP,       // current user's desktop
    // TODO: add more keys
    BASE_PATH_END
};

}   // namespace kbase

#endif // KBASE_BASE_PATH_PROVIDER_H_