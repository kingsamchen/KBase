/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_BASIC_MACROS_H_
#define KBASE_BASIC_MACROS_H_

#define DISALLOW_COPY(classname)                        \
    classname(const classname&) = delete;               \
    classname& operator=(const classname&) = delete

#define DISALLOW_MOVE(classname)                        \
    classname(classname&&) = delete;                    \
    classname& operator=(classname&&) = delete

#define DEFAULT_COPY(classname)                         \
    classname(const classname&) = default;              \
    classname& operator=(const classname&) = default;

#define DEFAULT_MOVE(classname)                         \
    classname(classname&&) = default;                   \
    classname& operator=(classname&&) = default;

#define UNUSED_VAR(x) \
    ::kbase::internal::SilenceUnusedVariableWarning(x)

#define DECLARE_DLL_FUNCTION(fn, type, dll) \
    auto fn = reinterpret_cast<type>(GetProcAddress(GetModuleHandleW(L##dll), #fn))

#define FORCE_AS_MEMBER_FUNCTION()                         \
    UNUSED_VAR(this)

// Put complicated implementation below.

namespace kbase {

namespace internal {

template<typename T>
void SilenceUnusedVariableWarning(T&&)
{}

}   // namespace internal

}   // namespace kbase

#endif  // KBASE_BASIC_MACROS_H_