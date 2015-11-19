/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_AT_EXIT_H_
#define KBASE_AT_EXIT_H_

#include <functional>
#include <mutex>
#include <stack>

#include "kbase\basic_macros.h"

namespace kbase {

class AtExitManager {
public:
    using AtExitCallback = std::function<void()>;

    AtExitManager();

    ~AtExitManager();

    DISALLOW_COPY(AtExitManager);

    DISALLOW_MOVE(AtExitManager);

    static void RegisterCallback(const AtExitCallback& callback);

private:
    static void ProcessCallbackNow();

private:
    std::mutex lock_;
    std::stack<AtExitCallback> callback_stack_;
    AtExitManager* next_at_exit_manager_;
};

}   // namespace kbase

#endif  // KBASE_AT_EXIT_H_