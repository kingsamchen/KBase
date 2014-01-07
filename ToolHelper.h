
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_TOOL_HELPER_
#define KBASE_TOOL_HELPER_

#include <functional>

#define SCOPE_GUARD_NAME_CAT(name, line) name##line
#define SCOPE_GUARD_NAME(name, line) SCOPE_GUARD_NAME_CAT(name, line)
#define ON_SCOPE_EXIT(callback) ScopeGuard SCOPE_GUARD_NAME(EXIT, __LINE__)(callback)

class ScopeGuard {
public:
    explicit ScopeGuard(std::function<void()> callback)
        : on_scope_exit_(callback), dismissed_(false)
    {
    }

    ~ScopeGuard()
    {
        if (!dismissed_) {
            on_scope_exit_();
        }
    }

    void Dismiss()
    {
        dismissed_ = true;
    }

private:
    std::function<void()> on_scope_exit_;
    bool dismissed_;
};

#define DISABLE_COPYABLE(TypeName) \
    TypeName(const TypeName&); \
    TypeName& operator =(const TypeName&);


#endif // KBASE_TOOL_HELPER_