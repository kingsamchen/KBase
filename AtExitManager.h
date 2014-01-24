// Author:  Kingsley Chen
// Date:    2014/01/25
// Purpose: object-orinted manager for at_exit

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_AT_EXIT_H_
#define KBASE_AT_EXIT_H_

#include <functional>
#include <mutex>
#include <stack>

#include "ToolHelper.h"

namespace KBase {

class AtExitManager {
public:
    typedef std::function<void(void*)> AtExitCallbackType;    

    AtExitManager();
    ~AtExitManager();

    static void RegisterCallback(const AtExitCallbackType& callback, void* param);
    static void ProcessCallbackNow();

private:
    DISABLE_COPYABLE(AtExitManager)

private:
    std::mutex lock_;
    std::stack<std::function<void()>> callback_stack_;
    AtExitManager* next_at_exit_manager_;
};

}   // namespace KBase

#endif  // KBASE_AT_EXIT_H_