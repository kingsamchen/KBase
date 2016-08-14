# String Views

[TOC]

### Motivations

`BasicStringView` and its specializations `StringView` and `WStringView` are resource **views** that refer to a constant contiguous sequence of character, like `std::string`; and it is inspired by `std::basic_string_view`, which is added since C++ 17.

### Views of Character-Sequence Like Objects

`BasicStringView` supports implicit conversion from `charT*`, `const charT*`, and `std::basic_string`, and is able to provide a uniform interface for these kinds of string types.

```c++
kbase::StringView sv = "hello world";
std::string str = "this is a test text";
kbase::StringView another_sv = str;
```

### String Operations

`BasicStringView` implements almost all operations provided by `std::basic_string`.

For a complete list, please refer to the class declaration.

### Compile-Time Operations

Some of `BasicStringView`'s operations can be done at compile time, only if the referenced string is a compile-time value:

```c++
constexpr kbase::StringView kStr = "hello, world";
constexpr bool not_empty = kStr.empty();
constexpr size_t length = kStr.length();
constexpr const char* data = kStr.data();
```

Note that, since Visual Studio 2015 only supports very basic features of `constexpr`, compile-time operations support of `BasicStringView` are very limited (doesn't even implement compile-time comparisons). But I think it is ok for most cases.

### Some Leaked Details

`BasicStringView` internally stores a **pointer** to the referenced string, and the **length** of its referenced part.

Therefore, a `BasicStringView` object is light that you can just pass it by value.

```c++
void SomeFunction(kbase::StringView str)
{
    // do something with str.
}
```