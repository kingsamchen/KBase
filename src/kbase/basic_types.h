/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_BASIC_TYPES_H_
#define KBASE_BASIC_TYPES_H_

#include <cstdint>
#include <string>

#include "kbase/basic_macros.h"

// Defines types that would be shared by among several files.
namespace kbase {

// `PathKey` is used by `PathService` and `BasePathProvider`.
using PathKey = int;

#if defined(OS_WIN)
using PathChar = wchar_t;
#else
using PathChar = char;
#endif

using PathString = std::basic_string<PathChar>;

using byte = uint8_t;

// Casts an enum value into an equivalent integer.
template<typename E>
constexpr auto enum_cast(E e)
{
    return static_cast<std::underlying_type_t<E>>(e);
}

}   // namespace kbase

#endif  // KBASE_BASIC_TYPES_H_