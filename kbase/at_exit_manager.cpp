/*
 @ Kingsley Chen
*/

#include "kbase/at_exit_manager.h"

#include <cstdio>
#include <cassert>

using std::lock_guard;
using std::mutex;

namespace kbase {

static AtExitManager* g_top_exit_manager = nullptr;

AtExitManager::AtExitManager() : next_at_exit_manager_(g_top_exit_manager)
{
    g_top_exit_manager = this;
}

AtExitManager::~AtExitManager()
{
    if (!g_top_exit_manager) {
        assert(false);
        // logging("try to ~AtExitManager without an AtExitManager");
        return;
    }

    assert(g_top_exit_manager == this);

    ProcessCallbackNow();
    g_top_exit_manager = next_at_exit_manager_;
}

// static
void AtExitManager::RegisterCallback(const AtExitCallbackType& callback, void* param)
{
    if (!g_top_exit_manager) {
        assert(false);
        // logging("try to RegisterCallback without an AtExitManager");
        return;
    }

    lock_guard<mutex> lock(g_top_exit_manager->lock_);
    g_top_exit_manager->callback_stack_.push(std::bind(callback, param));
}

// static
void AtExitManager::ProcessCallbackNow()
{
    if (!g_top_exit_manager) {
        assert(false);
        // logging("try to ProcessCallbackNow without an AtExitManager");
        return;
    }

    lock_guard<mutex> lock(g_top_exit_manager->lock_);

    while (!g_top_exit_manager->callback_stack_.empty()) {
        auto task = g_top_exit_manager->callback_stack_.top();
        task();
        g_top_exit_manager->callback_stack_.pop();
    }
}

}   // namespace kbase