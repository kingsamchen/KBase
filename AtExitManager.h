/************************************
** Edition: Demo
** Author:  Kingsley Chen
** Date:    2013/12/18
** Purpose: AtExitManager Declaration
************************************/

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

// provide a facility similar to the CRT atexit().
// actually is a imitation of base::AtExitManager

// the usage is simple. early in the main() or WinMain() scope 
// create an object on the stack:    
// int main()
// {
//     KBase::AtExitManager exitManager;
// }
// when the object goes out of scope, all registered callbacks
// and the singleton destructors will be called

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
    std::mutex _lock;
    std::stack<std::function<void()>> _callbackStack;
    AtExitManager* _nextAtExitManager;
};

}   // namespace KBase

#endif  // KBASE_AT_EXIT_H_