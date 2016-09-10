# AtExitManager

[TOC]

### Motivations

`AtExitManager` is somewhat a replacement for `atexit`, and is superior in two aspects:

- callback types
- execution time of callbacks under control

The callback types is actually `std::function<void()>` and hence any callable objects can be used as cleanup callbacks.

The second virtue is more important and useful, when you execute code in a DLL on Windows.

**Note:** We allow only one instance of `AtExitManager` in a module.

### Usages At Glance

The usage is simple, just create an object in an early scope of function `main()` or `WinMain()`, then register callback functions somewhere else afterwards. When the object goes out of scope, all registered callbacks will be called in LIFO order, just like `atexit`.

```c++
#include "kbase/at_exit_manager.h"

class C {
public:
    static void InitializeSomeResource()
    {
        if (!resource_) {
            resource_ = new Resource(...);
            kbase::AtExitManager::RegisterCallback([]() {
                reclaim(resource_);
            });
        }
    }

private:
    static Resource* resource_;
};

int main()
{
    kbase::AtExitManager exit_manager;
    // do sth...
    C::InitializeSomeResource();
    C c;
    // do sth...
	// the exit_manager will call reclaim(resource_) to clean up.
    return 0;
}
```