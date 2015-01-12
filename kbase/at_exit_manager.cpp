/*
 @ Kingsley Chen
*/

#include "kbase/at_exit_manager.h"

#include <cstdio>
#include <cassert>

#include "kbase\error_exception_util.h"

namespace kbase {

static AtExitManager* g_top_exit_manager = nullptr;

AtExitManager::AtExitManager() : next_at_exit_manager_(g_top_exit_manager)
{
    g_top_exit_manager = this;
}

AtExitManager::~AtExitManager()
{
    ENSURE(g_top_exit_manager == this)(reinterpret_cast<int>(g_top_exit_manager))
        (reinterpret_cast<int>(this)).raise();

    ProcessCallbackNow();
    g_top_exit_manager = next_at_exit_manager_;
}

// static
void AtExitManager::RegisterCallback(const AtExitCallbackType& callback)
{
    ENSURE(g_top_exit_manager).raise();

    std::lock_guard<std::mutex> lock(g_top_exit_manager->lock_);
    g_top_exit_manager->callback_stack_.push(callback);
}

// static
void AtExitManager::ProcessCallbackNow()
{
    ENSURE(g_top_exit_manager).raise();

    std::lock_guard<std::mutex> lock(g_top_exit_manager->lock_);

    while (!g_top_exit_manager->callback_stack_.empty()) {
        auto task = g_top_exit_manager->callback_stack_.top();
        task();
        g_top_exit_manager->callback_stack_.pop();
    }
}

}   // namespace kbase