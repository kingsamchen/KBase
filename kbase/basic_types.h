/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_BASIC_TYPES_H_
#define KBASE_BASIC_TYPES_H_

#include <string>

// Defines types that would be shared by among several files.
namespace kbase {

// |PathKey| is used by |PathService| and |BasePathProvider|.
typedef int PathKey;

typedef wchar_t PathChar;
typedef std::basic_string<PathChar> PathString;

}   // namespace kbase

#endif  // KBASE_BASIC_TYPES_H_