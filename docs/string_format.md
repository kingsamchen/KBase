# String Format

[TOC]

### Movitations

The string-format facility provides string formatting functions in two common styles, which could make the code more readable and succinct.

### printf-like formating functions

You can format a string in old-fasioned `printf`-like approaches:

```c++
#include "kbase/string_foramt.h"

// Return a string in given format.
auto str = kbase::StringPrintf("hello, %s", "0xCC");

// Modify a string in given format.
std::string str;
int segment_count = 0;
// do sth
kbase::StringPrintf(str, "%d segments found", segment_count);
```

### C# string-formating-like functions

It also support string formatting functions in style as in C#, or python.

They are more readable, type-safer, and user type friendly; any type that support `operator<<` can be used with these functions.

```c++
#include "kbase/string_format.h"

auto str = kbase::StringFormat("hello, {0}, welcome to {1}, you are our {2} client today", "0xCC", "bilibili", 1);

// with string "abc 00FF test def 3.1400 +255"
auto fancy_style = kbase::StringFormat("abc {0:0>4X} {1} def {2:.4} {0:+}", 255, "test", 3.14, 123);
```