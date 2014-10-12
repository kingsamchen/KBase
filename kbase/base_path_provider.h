
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_BASE_PATH_PROVIDER_H_
#define KBASE_BASE_PATH_PROVIDER_H_

#include "kbase\basic_types.h"

namespace kbase {

enum BasePathKey : PathKey {
    BASE_PATH_START = 0,
    DIR_CURRENT,            // current directory
    // TODO: add more keys
    BASE_PATH_END
};

}   // namespace kbase

#endif // KBASE_BASE_PATH_PROVIDER_H_