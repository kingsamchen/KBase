/************************************
** Edition: Demo
** Author:  Kingsley Chen
** Date:    2013/12/18
** Purpose: AtExitManager implementation
************************************/

#include "AtExitManager.h"

#include <cstdio>
#include <cassert>

using std::lock_guard;
using std::mutex;

namespace KBase {

static AtExitManager* g_topExitManager = nullptr;

AtExitManager::AtExitManager() : _nextAtExitManager(g_topExitManager)
{
    g_topExitManager = this;
}

AtExitManager::~AtExitManager()
{
    if (!g_topExitManager) {
        assert(false);
        // logging("try to ~AtExitManager without an AtExitManager");
        return;
    }

    assert(g_topExitManager == this);

    ProcessCallbackNow();
    g_topExitManager = _nextAtExitManager;
}

// static
void AtExitManager::RegisterCallback(const AtExitCallbackType& callback, void* param)
{
    if (!g_topExitManager) {
        assert(false);
        // logging("try to RegisterCallback without an AtExitManager");
        return;
    }

    lock_guard<mutex> lock(g_topExitManager->_lock);
    g_topExitManager->_callbackStack.push(std::bind(callback, param));
}

// static
void AtExitManager::ProcessCallbackNow()
{
    if (!g_topExitManager) {
        assert(false);
        // logging("try to ProcessCallbackNow without an AtExitManager");
        return;
    }

    lock_guard<mutex> lock(g_topExitManager->_lock);

    while (!g_topExitManager->_callbackStack.empty()) {
        auto task = g_topExitManager->_callbackStack.top();
        task();
        g_topExitManager->_callbackStack.pop();
    }
}

}   // namespace KBase