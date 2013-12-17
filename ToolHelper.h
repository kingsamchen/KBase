
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_TOOL_HELPER_
#define KBASE_TOOL_HELPER_

#include <functional>

#define SCOPE_GUARD_NAME_CAT(name, line) name##line
#define SCOPE_GUARD_NAME(name, line) SCOPE_GUARD_NAME_CAT(name, line)
#define ON_SCOPE_EXIT(callback) ScopeGuard SCOPE_GUARD_NAME(EXIT, __LINE__)(callback)

class ScopeGuard
{
    public:
        explicit ScopeGuard(std::function<void()> callback)
            : _onScopeExit(callback), _dismissed(false)
        {
        }

        ~ScopeGuard()
        {
            if (!_dismissed)
            {
                _onScopeExit();
            }
        }

        void Dismiss()
        {
            _dismissed = true;
        }

    private:
        std::function<void()> _onScopeExit;
        bool _dismissed;
};

#define DISABLE_COPYABLE(TypeName) \
    TypeName(const TypeName&); \
    TypeName& operator =(const TypeName&);


#endif // KBASE_TOOL_HELPER_