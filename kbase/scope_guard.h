/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SCOPE_GUARD_H_
#define KBASE_SCOPE_GUARD_H_

#include "kbase/basic_macros.h"

#define ON_SCOPE_EXIT \
    auto ANONYMOUS_VAR(_exit_) = ::kbase::internal::ScopeGuardDriver() + [&]() noexcept

#define MAKE_SCOPE_GUARD \
    ::kbase::internal::ScopeGuardDriver() + [&]() noexcept

namespace kbase {

template<typename F>
class ScopeGuard {
public:
    explicit ScopeGuard(F&& fn) noexcept
        : guard_fn_(std::forward<F>(fn)),
          dismissed_(false)
    {}

    // Overloaded operator+ on ScopeGuardDriver may rely on move-ctor.
    ScopeGuard(ScopeGuard&& other) noexcept
        : guard_fn_(std::move(other.guard_fn_)),
          dismissed_(other.dismissed_)
    {
        other.dismissed_ = true;
    }

    ~ScopeGuard()
    {
        if (!dismissed_) {
            guard_fn_();
        }
    }

    ScopeGuard(const ScopeGuard&) = delete;

    ScopeGuard& operator=(const ScopeGuard&) = delete;

    ScopeGuard& operator=(ScopeGuard&&) = delete;

    void Dismiss() noexcept
    {
        dismissed_ = true;
    }

private:
    F guard_fn_;
    bool dismissed_;
};

namespace internal {

struct ScopeGuardDriver {};

template<typename F>
auto operator+(ScopeGuardDriver, F&& fn)
{
    return ScopeGuard<F>(std::forward<F>(fn));
}

}   // namespace internal

}   // namespace kbase

#endif // KBASE_SCOPE_GUARD_H_
