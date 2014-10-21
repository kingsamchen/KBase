Error and Exception Utils
===

[TOC]

This util contains a set of handy tools for validation and error/exception issuing.

Members now consist of a macro `ENSURE`, a Windows last-error wrapper `LastError`, and a validating function `ThrowLastErrorIf` for Windows API invocations.

## ENSURE

`ENSURE` is an enhanced-version of `assert`. It supports rich-context-capture with nearly countless variables.

Unlike `assert`, if `ENSURE` asserts on failure, it throws a C++ exception, and use rich-context information it captured as description.

Moreover, `ENSURE` by default is available on **Release** mode too.

To make debug easier, `ENSURE` will internally invoke `assert`  on **Debug** mode, which gives you a chance to attach your debugger.

`ENSURE` can be used whenever `assert` is desire.

For code

```c++
void ParseCmdLine(int argc, char* argv[])
{
    ENSURE(argc >= 2)(argc)(argv[0]).raise();
    // do sth..
}

int main(int argc, char* argv[])
{
    try {
        ParseCmdLine(argc, argv);
        // do sth..
    } catch (std::exception& ex) {
        std::cerr << ex.what();
    }

    return 0;
}
```

If `ENSURE` fails in `ParseCmdLine`, an exception will be thrown (assuming in Release mode), along with description:

```
Failed: argc >= 2
File: c:\users\kingsley chen\documents\projects\cppdemo\cppdemo.cpp Line: 33
Current Variables:
    argc = 1
    argv[0] = C:\Users\Kingsley Chen\Documents\Projects\cppdemo\Debug\cppdemo.exe
```

`ENSURE` supports a switch on one-file granularity. The only you need to do is put the code

```c++
#define _ENSURE_DISABLED
```

**ahead** of including `ENSURE`'s header file.

## LastError

`LastError` is a thin-wrapper of Windows API `GetLastError`.

It automatically retrieves the last error code of the calling thread when constructing an instance, and stores the value internally.

If you want to obtain the text description corresponding to the error code, you can invoke `LastError::GetVerboseMessage`.

Usually, `LastError` is an auxiliary for implementing `ThrowLastErrorIf` function.

## ThrowLastErrorIf

`ThrowLastErrorIf` is designed as a intermediate validator for code that interfaces between exception and non-exception code.

This function has prototype

```c++
void ThrowLastErrorIf(bool expression, const std::string& user_message);
```

If the `expression` is evaluated as `false`, the function would throw an exception of type `Win32Exception`, which is derived from `std::runtime_error`.

The description of the exception contains verbose message of the last-error, and also with `user_message` attached.

```c++
void ReadFile(const wchar_t* file_name, std::string* data)
{
    HANDLE file = CreateFile(file_name,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             nullptr,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             nullptr);
    kbase::ThrowLastErrorIf(file == INVALID_HANDLE_VALUE, "cannot open the file");
    
    // read data from file
}


int _tmain(int argc, _TCHAR* argv[])
{
    try {
        std::string data;
        ReadFile(L"nonexisted_file", &data);
    } catch (kbase::Win32Exception& ex) {
        std::cerr << ex.what();
    }
    
    _getch();
    return 0;
}
```

If the file does not exist, the invocation of `CreateFile` will fail, and throw an exception, with message:

```
cannot open the file (The system cannot find the file specified.)
```