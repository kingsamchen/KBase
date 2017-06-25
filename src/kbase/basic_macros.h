/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_BASIC_MACROS_H_
#define KBASE_BASIC_MACROS_H_

#if defined(_WIN32) || defined(_WIN64)
#define OS_WIN 1
#elif defined(__linux__)
#define OS_POSIX 1
#else
#error Platform not supported
#endif

#if defined(_MSC_VER)
#define COMPILER_MSVC 1
#elif defined(__GNUC__)
#define COMPILER_GCC 1
#elif defined(__clang__)
#define COMPILER_CLANG 1
#else
#error Compiler not supported
#endif

#define DISALLOW_COPY(classname)                        \
    classname(const classname&) = delete;               \
    classname& operator=(const classname&) = delete

#define DISALLOW_MOVE(classname)                        \
    classname(classname&&) = delete;                    \
    classname& operator=(classname&&) = delete

// Default function is implicitly constexpr and noexcept, whenever it can be.

#define DEFAULT_COPY(classname)                         \
    classname(const classname&) = default;              \
    classname& operator=(const classname&) = default

#define DEFAULT_MOVE(classname)                         \
    classname(classname&&) = default;                   \
    classname& operator=(classname&&) = default

#define UNUSED_VAR(x)                                   \
    ::kbase::internal::SilenceUnusedVariableWarning(x)

#define CONCATENATE_IMPL(part1, part2) part1##part2
#define CONCATENATE(part1, part2) CONCATENATE_IMPL(part1, part2)
#define ANONYMOUS_VAR(tag) CONCATENATE(tag, __LINE__)

#define FORCE_AS_MEMBER_FUNCTION()                      \
    UNUSED_VAR(this)

#define FORCE_AS_NON_CONST_FUNCTION()                   \
    UNUSED_VAR(this)

#if defined(OS_WIN)
#define DECLARE_DLL_FUNCTION(fn, type, dll)                     \
    auto ANONYMOUS_VAR(_module_) = GetModuleHandleW(L##dll);    \
    auto fn = ANONYMOUS_VAR(_module_) ?                         \
                reinterpret_cast<type>(GetProcAddress(ANONYMOUS_VAR(_module_), #fn)) : nullptr
#endif

// Put complicated implementation below.

namespace kbase {

namespace internal {

template<typename T>
void SilenceUnusedVariableWarning(T&&)
{}

}   // namespace internal

}   // namespace kbase

#endif  // KBASE_BASIC_MACROS_H_