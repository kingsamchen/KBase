/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SECURE_C_RUNTIME_H_
#define KBASE_SECURE_C_RUNTIME_H_

#include "kbase/basic_macros.h"

#if defined(OS_POSIX)
#include <cstring>
#endif

#if defined(OS_POSIX)
#include "kbase/error_exception_util.h"
#endif

namespace kbase {

inline void* SecureMemcpy(void* dest, size_t dest_size_in_bytes, const void* src,
                          size_t count_to_copy)
{
#if defined(OS_WIN)
    memcpy_s(dest, dest_size_in_bytes, src, count_to_copy);
    return dest;
#else
    ENSURE(RAISE, count_to_copy <= dest_size_in_bytes)(dest_size_in_bytes)(count_to_copy).Require();
    return memcpy(dest, src, count_to_copy);
#endif
}

inline wchar_t* SecureMemcpy(wchar_t* dest, size_t dest_size_in_cch, const wchar_t* src,
                             size_t cch_to_copy)
{
#if defined(OS_WIN)
    wmemcpy_s(dest, dest_size_in_cch, src, cch_to_copy);
    return dest;
#else
    ENSURE(RAISE, cch_to_copy <= dest_size_in_cch)(dest_size_in_cch)(cch_to_copy).Require();
    return wmemcpy(dest, src, cch_to_copy);
#endif
}

}   // namespace kbase

#endif  // KBASE_SECURE_C_RUNTIME_H_
