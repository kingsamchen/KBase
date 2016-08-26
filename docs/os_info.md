# OSInfo

[TOC]

### Motivation

`OSInfo` provides capabilities for querying various system information, such as system version, processor architecture .etc.

### Check System Version

Sometimes, you need to know which version of the system your application is running on, because some APIs your code depends, might be unavailable untile a specific system version.

In this case, function `IsVersionOrGreater()` comes to rescue:

```c++
using kbase;
if (OSInfo::GetInstance()->IsVersionOrGreater(SystemVersion::WIN_8)) {
  // do sth only available since windows 8.
}
```

Moreover, if you want to know the current system version, in literal, you can call function `SystemVersionName()`:

```c++
std::cout << OSInfo::GetInstance()->SystemVersionName();	// maybe Windows 10
```

Not surprisingly, if there is a demand for which you need to know the exact current version number, probably for hacking something, `OSInfo` is still your friend, by using `version_number()`:

```c++
auto ver_number = OSInfo::GetInstance()->version_number();
```

**NOTE**: All functions above are not subject to application manifest settings; That means, they work well even on Windows 10.

### Misc Functions

Only take few instances here.

`OSInfo::UpTime()` tells you the duration, in *microseconds*, since the system boots, and this duration doesn't include time the system spends in sleep or hibernate.

```c++
auto up_seconds = OSInfo::UpTime() / 1000 / 1000;
```

If your application takes different behavior on 32-bit and 64-bit host systems, then you need to identify the architecture of the host system it runs on.

`OSInfo::GetInstance()->RunningOn64BitSystem()` solves this probolem.

### Some Leaked Details

`OSInfo` is a (**thread-safe**) singleton, since it caches most properties it has acquired, though it does provide some operations that are static to the class.

Note that, `OSInfo` now is a leaky singleton implementation, which means **it's destructor won't be called during exit of the process**. That's because I, so far, don't see any reason it's destructor must be called when exit.