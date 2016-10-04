# Stack Walker

[TOC]

### Motivations

C++ standard doesn't provide any facility to acquire callstack information, even when an exception is occurred.

However, we need callstack for diagnose when something goes wrong.

### Capture callstack from a given function call

```c++
std::string bazinga(std::string& log)
{
    log += " -> bazinga";
    std::cout << log << std::endl;
    kbase::StackWalker stack_walker;
    auto stack_trace = stack_walker.CallStackToString();
    return stack_trace;
}

std::string bar(std::string& log)
{
    log += " -> bar";
    return bazinga(log);
}

std::string foo()
{
    std::string log("foo");
    return bar(log);
}

int main()
{
    auto stack_trace = foo();
    std::cout << stack_trace << std::endl;
}
```

Note that, in some optimizing circumstances, you will get a  callstack different to how it appears in source code. It is normal and frequently happens.

Also be noted that, functions with internal linkage (i.e. *static functions*, or *functions defined in an anonymous namespace*) are not available in the captured callstack on POSIX systems (only module names are available), due to limitation of functions provided by the platform; While this is no such issue on Windows.

### Capture callstack from a SEH on Windows

class `StackWalker` provides another constructor for capturing callstack from an activated SEH.

```c++
StackWalker::StackWalker(CONTEXT* context) {}
```