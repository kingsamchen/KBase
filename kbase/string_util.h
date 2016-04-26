/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_STRING_UTIL_H_
#define KBASE_STRING_UTIL_H_

#include <vector>

#include "kbase/error_exception_util.h"
#include "kbase/string_view.h"

namespace kbase {

// Removes any characters specified by `remove_chars` from string `str`.

void RemoveChars(std::string& str, StringView remove_chars);
void RemoveChars(std::wstring& str, WStringView remove_chars);

// Replace `find_with` with `replace_with` in `str`.
// `pos` indicates where the search begins. if `pos` equals to `npos` or is greater
// than the length of `str`, these functions do nothing.
// If `relace_all` is not true, then only the first occurrence would be replaced.

void ReplaceSubstring(std::string& str,
                      StringView find_with,
                      StringView replace_with,
                      std::string::size_type pos = 0,
                      bool replace_all = true);
void ReplaceSubstring(std::wstring& str,
                      WStringView find_with,
                      WStringView replace_with,
                      std::wstring::size_type pos = 0,
                      bool replace_all = true);

// Removes characters in `trim_chars` in a certain range of `in`.
// `trim_chars` indicates characters that need to be removed from `in`.

void TrimString(std::string& str, StringView trim_chars);
void TrimString(std::wstring& str, WStringView trim_chars);

void TrimLeadingString(std::string& str, StringView trim_chars);
void TrimLeadingString(std::wstring& str, WStringView trim_chars);

void TrimTailingString(std::string& str, StringView trim_chars);
void TrimTailingString(std::wstring& str, WStringView trim_chars);

// Returns true, if the `str` is empty or contains only characters in `chars`;
// returns false, otherwise.

bool ContainsOnlyChars(const std::string& str, StringView chars);
bool ContainsOnlyChars(const std::wstring& str, WStringView chars);

// tolower and toupper are local sensitive, we might don't want to use them in some
// situations.
// it is legal to implicitly promotes a char to a wchar_t.
// following functions toggle case only for ascii-characters.

template<typename charT>
charT ToLowerASCII(charT ch)
{
    return (ch >= 'A' && ch <= 'Z') ? ch + 'a' - 'A' : ch;
}

template<typename charT>
charT ToUpperASCII(charT ch)
{
    return (ch >= 'a' && ch <= 'z') ? ch - 'a' + 'A' : ch;
}

template<typename strT>
void StringToLowerASCII(strT& str)
{
    for (auto it = str.begin(); it != str.end(); ++it) {
        *it = ToLowerASCII(*it);
    }
}

template<typename strT>
void StringToUpperASCII(strT& str)
{
    for (auto it = str.begin(); it != str.end(); ++it) {
        *it = ToUpperASCII(*it);
    }
}

// Converts the string to lower-case or upper-case.
// These functions support non-ASCII characters.

void StringToLower(std::string& str);
void StringToLower(std::wstring& str);

void StringToUpper(std::string& str);
void StringToUpper(std::wstring& str);

// Compares two strings in case-insensitive mode.
// These functions support non-ASCII characters, and are local sensitive.

int StringCompareCaseInsensitive(const std::string& x, const std::string& y);
int StringCompareCaseInsensitive(const std::wstring& x, const std::wstring& y);

// Compares two wide-string in case-insensitive mode.
// Not local sensitive
int SysStringCompareCaseInsensitive(const std::wstring& x, const std::wstring& y);

// Returns true, if `str` starts with `token`.
// Returns false, otherwise.

bool StartsWith(StringView str,
                StringView token,
                bool case_sensitive = true);
bool StartsWith(WStringView str,
                WStringView token,
                bool case_sensitive = true);

// Returns true, if `str` ends with `token`.
// Returns false, otherwise.

bool EndsWith(StringView str,
              StringView token,
              bool case_sensitive = true);
bool EndsWith(WStringView str,
              WStringView token,
              bool case_sensitive = true);

// Set up enough memory in `str` to accomodate a c-style string with length
// of `length_including_null`. be wary of that real size of the string data
// does not count the null - terminate character. this function is useful when
// interaction between a string object and a legacy API is required.
// Returns pointer to the underlying data of the string object.
template<typename strT>
typename strT::value_type* WriteInto(strT& str, size_t length_including_null)
{
    // If the length is equal to 1, the underlying string size is 0,
    // operator[] may then result in an undefined behavior in this situation.
    ENSURE(CHECK, length_including_null > 1U).Require();
    str.reserve(length_including_null);
    str.resize(length_including_null - 1);

    return &str[0];
}

// Split a string into fields delimieted by any of the characters in `delimiters`.
// Fields are added into `tokens`.
// Returns the number of tokens found.

size_t SplitString(const std::string& str,
                   StringView delimiters,
                   std::vector<std::string>& tokens);
size_t SplitString(const std::wstring& str,
                   WStringView delimiters,
                   std::vector<std::wstring>& tokens);

// Combines string parts in `tokens` by using `sep` as separator.
// Returns combined string.

std::string JoinString(const std::vector<std::string>& tokens,
                       StringView sep);
std::wstring JoinString(const std::vector<std::wstring>& tokens,
                        WStringView sep);

// Pattern matching algorithm, also supports wildcards, in case-sensitive mode.
// metacharacter `?` matches exactly one character unless the character is a `.`
// metacharacter `*` matches any sequence of zero or more characters.
bool MatchPattern(const std::string& str, const std::string& pat);
bool MatchPattern(const std::wstring& str, const std::wstring& pat);

// Determines if all characters in string are defined in ASCII code page.
bool IsStringASCII(StringView str);
bool IsStringASCII(WStringView str);

template<typename T>
struct ToUnsigned {
    using Unsigned = T;
};

template<>
struct ToUnsigned<char> {
    using Unsigned = unsigned char;
};

template<>
struct ToUnsigned<signed char> {
    using Unsigned = unsigned char;
};

template<>
struct ToUnsigned<wchar_t> {
    using Unsigned = unsigned short;
};

}   // namespace kbase

#endif  // KBASE_STRING_UTIL_H_