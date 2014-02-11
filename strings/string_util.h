// Author:  Kingsley Chen
// Date:    2014/02/11
// Purpose: exposed interfaces of string util collection.

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_STRINGS_STRING_UTIL_H_
#define KBASE_STRINGS_STRING_UTIL_H_ 

#include <string>
#include <vector>

namespace KBase {

namespace StringUtil {

/*
 @ brief
    remove any characters specified by |remove_chars| in string |in|.
 @ return
    true, if any charaters are removed.
    false, otherwise.
*/

bool RemoveChars(const std::string& in,
                 const char remove_chars[],
                 std::string* out);
bool RemoveChars(const std::wstring& in,
                 const wchar_t remove_chars[],
                 std::wstring* out);

/*
 @ brief
    replace |find_with| with |replace_with| in |str|.
 @ params
    pos[in] indicates the search beginning. if |pos| equals to |npos| or is
    greater than the length of |str|, these functions do nothing.
*/

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

/*
 @ brief
    remove characters in |trim_chars| in a certain range of |in|.
 @ params
    trim_chars[in] charaters that need to be removed from |in|. must be
    null-terminated.
 @ return
    true, if having trimed.
    false, otherwise.
*/

bool TrimString(const std::string& in, const char trim_chars[], std::string* out);
bool TrimString(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out);

bool TrimLeadingStr(const std::string& in, const char trim_chars[], std::string* out);
bool TrimLeadingStr(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out);

bool TrimTailingStr(const std::string& in, const char trim_chars[], std::string* out);
bool TrimTailingStr(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out);

/*
 @ return
    true, if the |in| is empty or contains only characters in |chars|.
    false, otherwise.
*/

bool ContainsOnlyChars(const std::string& in, const char chars[]);
bool ContainsOnlyChars(const std::wstring& in, const wchar_t chars[]);

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

/*
 @ return
    true, if |str| starts with |token|.
    false, otherwise.
*/

bool StartsWith(const std::string& str, 
                const std::string& token, 
                bool case_sensitive = true);
bool StartsWith(const std::wstring& str, 
                const std::wstring& token, 
                bool case_sensitive = true);

/*
 @ return
    true, if |str| ends with |token|.
    false, otherwise.
*/

bool EndsWith(const std::string& str,
              const std::string& token,
              bool case_sensitive = true);
bool EndsWith(const std::wstring& str,
              const std::wstring& token,
              bool case_sensitive = true);

/*
 @ brief
    set up enough memory in |str| to accomodate a c-style string with length 
    of |length_including_null|. be wary of that real size of the string data
    does not count the null-terminate character. this function is useful when
    interaction between a string object and a legacy API is required.
 @ return
    pointer to the underlying data of a string object.
*/
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

/*
 @ brief
    split a string into fields delimieted by any of the characters in
    |delimiters|. fields are added into |tokens|.
 @ return
    the number of tokens found.
*/

size_t Tokenize(const std::string& str,
                const std::string& delimiters,
                std::vector<std::string>* tokens);
size_t Tokenize(const std::wstring& str,
                const std::wstring& delimiters,
                std::vector<std::wstring>* tokens);

}   // namespace StringUtil

}   // namespace KBase

#endif  // KBASE_STRINGS_STRING_UTIL_H_