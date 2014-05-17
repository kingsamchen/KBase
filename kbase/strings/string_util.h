/*
 @ Kingsley Chen
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_STRINGS_STRING_UTIL_H_
#define KBASE_STRINGS_STRING_UTIL_H_ 

#include <cassert>
#include <string>
#include <vector>

#include "kbase/strings/string_piece.h"

namespace kbase {

// Removes any characters specified by |remove_chars| in string |in|.
// Returns true, if any characters are removed; return false, otherwise. 

bool RemoveChars(const std::string& in,
                 const char remove_chars[],
                 std::string* out);
bool RemoveChars(const std::wstring& in,
                 const wchar_t remove_chars[],
                 std::wstring* out);

// Replace |find_with| with |replace_with| in |str|.
// |pos| indicates the search beginning. if |pos| equals to |npos| or is greater
// than the length of |str|, these functions do nothing.

void ReplaceSubstr(std::string* str,
                   const std::string& find_with,
                   const std::string& replace_with,
                   std::string::size_type pos = 0);
void ReplaceSubstr(std::wstring* str,
                   const std::wstring& find_with,
                   const std::wstring& replace_with,
                   std::wstring::size_type pos = 0);

void ReplaceFirstSubstr(std::string* str,
                        const std::string& find_with,
                        const std::string& replace_with,
                        std::string::size_type pos = 0);
void ReplaceFirstSubstr(std::wstring* str,
                        const std::wstring& find_with,
                        const std::wstring& replace_with,
                        std::wstring::size_type pos = 0);

// Removes characters in |trim_chars| in a certain range of |in|. 
// |trim_chars| indicates characters that need to be removed from |in|. must be
// null-terminated.
// Returns true if having trimed; otherwise returns false.

bool TrimString(const std::string& in, const char trim_chars[], std::string* out);
bool TrimString(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out);

bool TrimLeadingStr(const std::string& in, const char trim_chars[], std::string* out);
bool TrimLeadingStr(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out);

bool TrimTailingStr(const std::string& in, const char trim_chars[], std::string* out);
bool TrimTailingStr(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out);

// Returns true, if the |in| is empty or contains only characters in |chars|;
// returns false, otherwise.

bool ContainsOnlyChars(const std::string& in, const char chars[]);
bool ContainsOnlyChars(const std::wstring& in, const wchar_t chars[]);

// tolower and toupper are local sensitive, we might don't want to use them in some
// situations.
// it is legal to implicitly promotes a char to a wchar_t.
// following functions toggle case only for ascii-characters.

template<typename charT>
inline charT ToLowerASCII(charT ch)
{
    return (ch >= 'A' && ch <= 'Z') ? ch + 'a' - 'A' : ch;
}

template<typename charT>
inline charT ToUpperASCII(charT ch)
{
    return (ch >= 'a' && ch <= 'z') ? ch - 'a' + 'A' : ch;
}

template<typename strT>
void StringToLowerASCII(strT* str)
{
    for (auto it = str->begin(); it != str->end(); ++it) {
        *it = ToLowerASCII(*it);
    }
}

template<typename strT>
strT StringToLowerASCII(const strT& str)
{
    strT tmp(str);
    StringToLowerASCII(&tmp);
    return tmp;
}

template<typename strT>
void StringToUpperASCII(strT* str)
{
    for (auto it = str->begin(); it != str->end(); ++it) {
        *it = ToUpperASCII(*it);
    }
}

template<typename strT>
strT StringToUpperASCII(const strT& str)
{
    strT tmp(str);
    StringToUpperASCII(&tmp);
    return tmp;
}

// Converts the string to lower-case or upper-case.
// These functions support non-ASCII characters.

void StringToLower(std::string* str);
std::string StringToLower(const std::string& str);

void StringToLower(std::wstring* str);
std::wstring StringToLower(const std::wstring& str);

// Compares two strings in case-insensitive mode.
// These functions support non-ASCII characters, and are local sensitive.

int StringCompareCaseInsensitive(const std::string& x, const std::string& y);
int StringCompareCaseInsensitive(const std::wstring& x, const std::wstring& y);

// Compares two wide-string in case-insensitive mode.
// Not local sensitive
int SysStringCompareCaseInsensitive(const std::wstring& x, const std::wstring& y);

// Returns true, if |str| starts with |token|.
// Returns false, otherwise.

bool StartsWith(const std::string& str, 
                const std::string& token, 
                bool case_sensitive = true);
bool StartsWith(const std::wstring& str, 
                const std::wstring& token, 
                bool case_sensitive = true);

// Returns true, if |str| ends with |token|.
// Returns false, otherwise.

bool EndsWith(const std::string& str,
              const std::string& token,
              bool case_sensitive = true);
bool EndsWith(const std::wstring& str,
              const std::wstring& token,
              bool case_sensitive = true);

// Set up enough memory in |str| to accomodate a c-style string with length 
// of | length_including_null | .be wary of that real size of the string data
// does not count the null - terminate character. this function is useful when
// interaction between a string object and a legacy API is required.
// Returns pointer to the underlying data of the string object.
template<typename strT>
inline typename strT::value_type* WriteInto(strT* str, 
                                            size_t length_including_null)
{
    // if the length is equal to 1, the underlying string size is 0,
    // operator[] may then result in an undefined behavior in this situation.
    assert(length_including_null > 1U);
    str->reserve(length_including_null);
    str->resize(length_including_null - 1);

    return (&(*str)[0]);
}

// Split a string into fields delimieted by any of the characters in
// |delimiters| .fields are added into |tokens|.
// Returns the number of tokens found.

size_t Tokenize(const std::string& str,
                const std::string& delimiters,
                std::vector<std::string>* tokens);
size_t Tokenize(const std::wstring& str,
                const std::wstring& delimiters,
                std::vector<std::wstring>* tokens);

// Combines string parts in |tokens| by using |sep| as separator.
// Returns combined string.

std::string JoinString(const std::vector<std::string>& tokens,
                       char sep);
std::wstring JoinString(const std::vector<std::wstring>& tokens,
                        wchar_t sep);
std::string JoinString(const std::vector<std::string>& tokens,
                       const std::string& sep);
std::wstring JoinString(const std::vector<std::wstring>& tokens,
                        const std::wstring& sep);

// Pattern matching algorithm, also supports wildcards, in case-sensitive mode.
// metacharacter |?| matches exactly one character unless the character is a |.|
// metacharacter |*| matches any sequence of zero or more characters.
bool MatchPattern(const std::string& str, const std::string& pat);
bool MatchPattern(const std::wstring& str, const std::wstring& pat);

// Determines if all characters in string are defined in ASCII code page.
bool IsStringASCII(const StringPiece& str);
bool IsStringASCII(const WStringPiece& str);

template<typename T>
struct ToUnsigned {
    typedef T Unsigned;
};

template<>
struct ToUnsigned<char> {
    typedef unsigned char Unsigned;
};

template<>
struct ToUnsigned<signed char> {
    typedef unsigned char Unsigned;
};

template<>
struct ToUnsigned<wchar_t> {
    typedef unsigned short Unsigned;
};

}   // namespace kbase

#endif  // KBASE_STRINGS_STRING_UTIL_H_