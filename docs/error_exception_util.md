# Error and Exception Utils

[TOC]

### Motivation

Error-and-exception-util is one of the infrastructural modules of kbase.

This facility provides capabilities for **data/class invariant validation**, and would take actions, like throwing an exception, or writing a minidump file, when the invariant is not held, or some rules are breached.

That is, a module came up for programming by contract.



### The Macro ENSURE

The macro `ENSURE` is the core of this facility.

The syntax of using this macro for invariant validation is as follows:

``` c++
ENSURE(ACTION, Condition)(var1)(var2)...(vark).Require(["some extra message"]);
```

Note: *__[]__ denotes optional parameter*

If `Condition` is true, nothing would happen(i.e. no runtime penalty in this case); Otherwise, the specified `ACTION` is performed, with information about captured variables, and the extra message, if given, attached.

There are three actions at your disposal:

- Check
- Raise
- RaiseWithDump

`CHECK`, like `assert`, works **only in debug mode**, and does nothing in release mode. When it is invoked, it displays an error dialog with detailed message about this violation; and then, if a debugger is attached, the execution is interrupted as if a breakpoint is reached.

Moreover, the error message that was just displayed contains the (*symbol* resolved if possible) entire *callstack* for helping diagnosis.

And by default, even you use other actions, the action `CHECK` is performed (*in debug mode*) first, to give you a chance to examine the problem in the primary scene.

To silence `CHECK` in other actions, call

``` c++
AlwaysCheckForEnsureInDebug(false);
```



`RAISE` would throw an exception when the condition is violated.

By default, the exception type, in this case, is `std::runtime_error`; Exception message also includes callstack in site.

Exceptions issued in this way indicate a **_don't-know-how-to-solve_** failure, and it should be rare.

For **intended exceptions that you already knew how to handle them**, you can use an variant:

```c++
ENSURE(RAISE, condition)(some_var).Require<MyException>(["extra message"]);
```

`MyException` here is an user customized exception class that **derives from** `std::exception`, and it provides a specific failure semantic, on purpose by developers themselves.

Please be noted that, the exception message in this case doesn't include callstack, because it is a completely different case.

Moreover, you can specify your customized class only with **RAISE** action; using with other actions is simply ignored and has none real effect.



`RAISE_WITH_DUMP` would first create a minidump file at the given path (specified by `SetMiniDumpDirectory(some_path)`, and using current working directory if no specific path was given), and then throws an exception of type `ExceptionWithMiniDump`, which has a member function `GetMiniDumpPath()` for locating the dump file.

Note that, function `SetMiniDumpDirectory` is not thread-safe, so you should better call it at the start of the program.

``` c++
#include "kbase/file_path.h"
#include "kbase/error_exception_util.h"

int main()
{
    kbase::AlwaysCheckForEnsureInDebug(false);
    kbase::SetMiniDumpDirectory(kbase::FilePath(L"c:\\crash_dump"));
    ...
}
```

Action `RAISE_WITH_DUMP` is much like normal `RAISE`, but intending for more strict or important case.

Exception message in class `ExceptionWithMiniDump` doesn't include callstack, because it is already contained in the mini-dump file, which also offers a little more diagnostic data.



### The LastError Wrapper

class `LastError` is a bridge to interacting with Windows APIs.

`LastError` automatically retrieves the last error code of the calling thread(set by a Windows API that was called) when constructing an instance.

It's function `GetDescriptiveMessage()` translates error code to human-understandable text; and the class overload `operator<<`, which outputs error code along with descriptive message.

``` c++
BOOL rv = CallSomeWindowsAPI(...);
ENSURE(RAISE, rv == TRUE)(kbase::LastError()).Require("SomeWindowsAPI faield");
```



### The NotReached Shortcut

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

