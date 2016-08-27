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

#define NAME_CAT(prefix, tag) prefix##tag
#define MODULE_VAR(tag) NAME_CAT(_module_, tag)
#define DECLARE_DLL_FUNCTION(fn, type, dll)                 \
    auto MODULE_VAR(__LINE__) = GetModuleHandleW(L##dll);   \
    auto fn = MODULE_VAR(__LINE__) ? reinterpret_cast<type>(GetProcAddress(MODULE_VAR(__LINE__), #fn)) : nullptr

#define FORCE_AS_MEMBER_FUNCTION()                          \
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