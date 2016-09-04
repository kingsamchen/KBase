# String Encoding Conversions

[TOC]

### Motivations

Windows uses utf-16 (via data type `wchar_t`, and whose size differs with which on POSIX platforms) extensively for its Win32 APIs.

So it is inevitable that we will do encoding conversions between utf-8 and utf-16 when we interact with Win32 APIs.

This facility provides easy ways to convert encoding of a string between utf-8 and utf-16.

### Conversions

Interfaces are ease of use

```c++
// utf-8 -> utf-16
std::string utf8_str = "blahblah";
std::wstring utf16_str = kbase::UTF8ToWide(utf8_str);
SomeWindowsAPI(utf_str);

// utf-16 -> utf-8
std::wstring utf16_str = L"blahblah";
std::string utf8_str = kbase::WideToUTF8(utf16_str);
```

Sometimes, we want to convert a `std::string`, which contains ASCII characters only, to `std::wstring`, or vice versa, without real encoding conversion, because it will incur unnecessary performance penalty.

Use `kbase::ASCIIToWide()` and `kbase::WideToASCII()` in this case.

### Some Leaked Details

encoding conversions rely on `std::wstring_convert()` and `std::codecvt_utf8_utf16`, which both are introduced since C++ 11.