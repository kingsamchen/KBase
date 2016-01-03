/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SCOPE_GUARD_H_
#define KBASE_SCOPE_GUARD_H_

#include <functional>

#include "kbase/basic_macros.h"

namespace kbase {

#define SCOPE_GUARD_NAME_CAT(name, line) name##line
#define SCOPE_GUARD_NAME(name, line) SCOPE_GUARD_NAME_CAT(name, line)
#define ON_SCOPE_EXIT(callback) \
    ::kbase::ScopeGuard SCOPE_GUARD_NAME(EXIT, __LINE__)(callback)

class ScopeGuard {
public:
    explicit ScopeGuard(std::function<void()> callback)
        : on_scope_exit_(callback), dismissed_(false)
    {}

    ~ScopeGuard()
    {
        if (!dismissed_) {
            on_scope_exit_();
        }
    }

    DISALLOW_COPY(ScopeGuard);

    DISALLOW_MOVE(ScopeGuard);

    void Dismiss()
    {
        dismissed_ = true;
    }

private:
    std::function<void()> on_scope_exit_;
    bool dismissed_;
};

}   // namespace kbase

#endif // KBASE_SCOPE_GUARD_H_