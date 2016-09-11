# Scope Guard

[TOC]

### Movitations

`ScopeGuard` provides yet another option to enforce RAII-idiom for a specific resource, while not having to make a complete RAII-wrapper.

It is especially convenient for interacting with native resources.

### Execute On Exit

```c++
#include "kbase/scope_guard.h"

void SomeFunction()
{
    auto r = AllocateSomeResource();
    ON_SCOPE_EXIT {
        // will execute when goes out of the function.
        ReleaseSomeResource(r);
    };

    DoSometing(r);
}
```

Be noted that, all variables used within the guard are captured by reference.

Besides, the statements inside the guard are better to be `noexcept`, which aslo matches cleanup semantics.

### Halt the Execution If Necessary

Sometimes, we need to pull off the exit-callback when a certain conditions meet.

`ScopeGuard` offers you a `Dismiss()` method to work on this:

```c++
std::vector<int> v {1, 3, 5};

{
    auto guard = MAKE_SCOPE_GUARD { v.clear(); };
    if (v.size() < 5) {
        guard.Dismiss();
    }
}

ASSERT_TRUE(v.size() == 3);

v.push_back(7);
v.push_back(9);

{
    auto guard = MAKE_SCOPE_GUARD { v.clear(); };
    if (v.size() < 5) {
        guard.Dismiss();
    }
}

EXPECT_TRUE(v.empty());
```