# Error and Exception Utils

[TOC]

### Motivation

Error-and-exception-util is one of the infrastructural modules of kbase.

This facility provides capabilities for data/class invariant validation, and would take actions, like throwing an exception, or writing a minidump file, when the invariant is not held, or some rules are breached.

That is, a module came up for programming by contract.



### The Macro ENSURE

The macro `ENSURE` is the core of this facility.

The syntax of using this macro for invariant validation is as follows:

``` c++
ENSURE(ACTION, Condition)(var1)(var2)...(vark).Require(["some extra message"]);
```

Note: *__[]__ denotes optional parameter*

If `Condition` is true, nothing would happen(i.e. no runtime penalty in this case); Otherwise, the specified `ACTION` is performed, with information about captured variables, and the extra message, if given, attached.

There are three actions at your disposal: Check, Raise, and RaiseWithDump.

`CHECK` is like `assert`, works only in debug mode, and does nothing in release mode. When it is invoked, it displays an error dialog with detailed message about this violation; and then, if a debugger is attached, the execution is interrupted as if a breakpoint is reached.

Moreover, the error message that was just displayed contains the (symbol resolved if possible) entire callstack for helping diagnosis.

And by default, even you use other actions, the action `CHECK` is performed (*in debug mode*) first, to give you a chance to examine the problem in the primary scene.

To silence `CHECK` in other actions, call

``` c++
EnableAlwaysCheckForEnsureInDebug(false);
```

`RAISE` would throw an exception when condition is violated.

`RAISE_WITH_DUMP` would first create a minidump file at the given path (specified by `SetMiniDumpDirectory(some_path)`, and using current working directory if no specific path was given), and then throws an exception of type `ExceptionWithMiniDump`, which has a member function `GetMiniDumpPath()` for locating the dump file.

Note that, functions `SetMiniDumpDirectory` and `EnableAlwaysCheckForEnsureInDebug` are not thread-safe, so you should call them at the start of the program.

``` c++
#include "kbase/file_path.h"
#include "kbase/error_exception_util.h"

int main()
{
    kbase::EnableAlwaysCheckForEnsureInDebug(false);
    kbase::SetMiniDumpDirectory(kbase::FilePath(L"c:\\crash_dump"));
    ...
}
```



### Companions

class `LastError` is a bridge to interacting with Windows APIs.

`LastError` automatically retrieves the last error code of the calling thread(set by a Windows API that was called) when constructing an instance.

It's function `GetDescriptiveMessage()` translates error code to human-understandable text; and the class overload `operator<<`, which outputs error code along with descriptive message.

``` c++
BOOL rv = CallSomeWindowsAPI(...);
ENSURE(RAISE, rv == TRUE)(kbase::LastError()).Require("SomeWindowsAPI faield");
```



When you need to ensure some code pathes shouldn't be reached, you can use the compile-time function: `NotReached()`.

This function always returns false, and no runtime penalty.

``` c++
switch (state) {
    case STATE_INITIAL:
        // do sth.
        break;
    case STATE_RUNNING:
        // do sth.
        break;
    case STATE_TERMINATED:
        // do sth.
        break;
    default:
        ENSURE(CHECK, kbase::NotReached())(state).Require();
}
```

