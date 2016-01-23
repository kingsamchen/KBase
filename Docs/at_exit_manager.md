AtExitManager
===

`AtExitManager` is somewhat a replacement in our code level for `atexit`, its kin that exists since the age of C.

`AtExitManager` is superior due to its callback type:

```c++
using AtExitCallback = std::function<void()>;
```

which means we can use a lambda with captured state as our exit callback function, thus being more practical.

`kbase::Singleton` also relies on `AtExitManager` to reclaim memory before termination.

The usage is simple, create an object in an early scope of function `main()` or `WinMain()`, then register callback functions somewhere else afterwards. When the object goes out of scope, all registered callbacks will be called in LIFO order, just like `atexit`.

```c++
#include "kbase\at_exit_manager.h"

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
    kbase::InitLoggingSettings(kbase::LoggingSettings());   // see logging.h
    // do sth...
    C::InitializeSomeResource();
    C c;
    // do sth...
	// the exit_manager will call reclaim(resource_) to clean up.
    return 0;
}
```

**Note**: Normally, there is **only one** object instance for `AtExitManager` in a single module. Multiple instances will cause assert failed in DEBUG mode.