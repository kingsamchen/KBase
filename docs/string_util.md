# String Util Functions

[TOC]

## Introduction

Module unit `string_util` provides several handy functions for convenient string processing.

These functions are not exposed as templates, instead, they are specifically customized for `std::string` and `std::wstring`, which are mostly used in daily development.

## Quick reference to util functions

### Erasing characters from a string

```c++
void EraseChars(std::string& str, StringView chars);
void EraseChars(std::wstring& str, WStringView chars);

std::string EraseCharsCopy(const std::string& str, StringView chars);
std::wstring EraseCharsCopy(const std::wstring& str, WStringView chars);
```

`EraseChars()` allows you to remove every character designated by `chars` from the `str`.

```c++
std::string str = "hello world";
EraseChars(str, "ol");
REQUIRE(str == std::string("he wrd"));
```

`EraseCharsCopy()` does the modification on a copy.

### Replacing a sub-string

```c++
void ReplaceString(std::string& str,
                   StringView find_with,
                   StringView replace_with,
                   std::string::size_type pos = 0,
                   bool replace_all = true);
void ReplaceString(std::wstring& str,
                   WStringView find_with,
                   WStringView replace_with,
                   std::wstring::size_type pos = 0,
                   bool replace_all = true);

std::string ReplaceStringCopy(const std::string& str,
                              StringView find_with,
                              StringView replace_with,
                              std::string::size_type pos = 0,
                              bool replace_all = true);
std::wstring ReplaceStringCopy(const std::wstring& str,
                               WStringView find_with,
                               WStringView replace_with,
                               std::wstring::size_type pos = 0,
                               bool replace_all = true);
```

`ReplaceString()` replaces `find_with` with `replace_with` in `str`.

`pos` indicates where the search begins; if `pos` equals to `npos` or is greater than the length of `str`, the function does nothing.

If `relace_all` is not true, then only the first occurrence would be replaced.

```c++
std::string str = "This is a test text for string replacing unittest";

// case 1
ReplaceString(str, "test", "t-e-s-t");
REQUIRE(str == std::string("This is a t-e-s-t text for string replacing unitt-e-s-t"));

// case 2
ReplaceString(str, "is", "ere", 0, false);
REQUIRE(str == std::string("There is a test text for string replacing unittest"));
```

`ReplaceStringCopy()` is like `ReplaceString()`, but does the modification on a copy.

### Triming string

trim-string series functions allow you to trim leading characters, trailing characters, or both for a string.

```c++
void TrimString(std::string& str, StringView chars);
void TrimString(std::wstring& str, WStringView chars);

std::string TrimStringCopy(const std::string& str, StringView chars);
std::wstring TrimStringCopy(const std::wstring& str, WStringView chars);

void TrimLeadingString(std::string& str, StringView chars);
void TrimLeadingString(std::wstring& str, WStringView chars);

std::string TrimLeadingStringCopy(const std::string& str, StringView chars);
std::wstring TrimLeadingStringCopy(const std::wstring& str, WStringView chars);

void TrimTailingString(std::string& str, StringView chars);
void TrimTailingString(std::wstring& str, WStringView chars);

std::string TrimTailingStringCopy(const std::string& str, StringView chars);
std::wstring TrimTailingStringCopy(const std::wstring& str, WStringView chars);
```

Similarly, each of them has a corresponding copy-version, which does the modification on a copy.

### Containing only characters

The function returns `true`, if the `str` is **empty**, or contains only characters in `chars`; it returns `false`, otherwise.

```c++
bool ContainsOnlyChars(StringView str, StringView chars);
bool ContainsOnlyChars(WStringView str, WStringView chars);
```

### Toggling case for ASCII characters

These functions are able to toggle an ASCII character into lower-case or upper-case.

Note: the `str` should contain ASCII characters only!

```c++
void ASCIIStringToLower(std::string& str);
void ASCIIStringToLower(std::wstring& str);

std::string ASCIIStringToLowerCopy(const std::string& str);
std::wstring ASCIIStringToLowerCopy(const std::wstring& str);

void ASCIIStringToUpper(std::string& str);
void ASCIIStringToUpper(std::wstring& str);

std::string ASCIIStringToUpperCopy(const std::string& str);
std::wstring ASCIIStringToUpperCopy(const std::wstring& str);
```

Copy-versions of the function do the modifications on copies.

### String being ASCII-only

The function returns `true`, if every characters in `str` is an ASCII character.

```c++
bool IsStringASCIIOnly(StringView str);
bool IsStringASCIIOnly(WStringView str);
```

### ASCII-string case insensitive comparison

These functions allow you to compare two ASCII-strings in a case-insensitive way.

```c++
//  < 0 (lhs < rhs)
//  = 0 (lhs == rhs)
//  > 0 (lhs > rhs)
int ASCIIStringCompareCaseInsensitive(StringView lhs, StringView rhs);
int ASCIIStringCompareCaseInsensitive(WStringView lhs, WStringView rhs);

bool ASCIIStringEqualCaseInsensitive(StringView lhs, StringView rhs);
bool ASCIIStringEqualCaseInsensitive(WStringView lhs, WStringView rhs);
```

### Prefix/suffix matching

The functions allow you to check if a string matches with a given prefix or suffix.

```c++
bool StartsWith(StringView str,
                StringView token,
                CaseMode mode = CaseMode::Sensitive);
bool StartsWith(WStringView str,
                WStringView token,
                CaseMode mode = CaseMode::Sensitive);

bool EndsWith(StringView str,
              StringView token,
              CaseMode mode = CaseMode::Sensitive);
bool EndsWith(WStringView str,
              WStringView token,
              CaseMode mode = CaseMode::Sensitive);
```

By default, the matching is case sensitive; but it also supports ASCII-insensitive:

```c++
enum class CaseMode {
    Sensitive,
    ASCIIInsensitive
};
```

### Spliting and joining a string

```c++
size_t SplitString(StringView str, StringView delimiters, std::vector<std::string>& tokens);
size_t SplitString(WStringView str, WStringView delimiters, std::vector<std::wstring>& tokens);
```

`SplitString()` split a string, delimieted by any of the characters in `delimiters`, into fields.

Fields are added into `tokens` and returns the number of tokens found.

```c++
std::string JoinString(const std::vector<std::string>& tokens, StringView sep);
std::wstring JoinString(const std::vector<std::wstring>& tokens, WStringView sep);
```

`JoinString()` does the other way around: it combines tokens into a string, concatenating with `sep`

### Using string as a buffer

```c++
template<typename strT>
typename strT::value_type* WriteInto(strT& str, size_t length_including_null);
```

When you want interactions between a string object and a legacy API, this function can do help.

The function sets up memory space for given `length_including_null` size, and returns a pointer to that underlying buffer.

Note: the `length_including_null` must be greater than 1.

### Wildcard matching

```c++
bool MatchPattern(const std::string& str, const std::string& pat);
bool MatchPattern(const std::wstring& str, const std::wstring& pat);
```

metacharacter `?` matches exactly one character unless the character is a `.`

metacharacter `*` matches any sequence of zero or more characters.

The matching is case sensitive.