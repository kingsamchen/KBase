// Author:  Kingsley Chen
// Date:    2014/01/28
// Purpose: exposed interfaces of string util collection.

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_STRINGS_STRING_UTIL_H_
#define KBASE_STRINGS_STRING_UTIL_H_ 

#include <string>

namespace KBase {

namespace StringUtil {

/*
 @ brief:
    remove any characters specified by |remove_chars| in string |in|
 @ return:
    true, if any charaters are removed
    false, otherwise
*/

bool RemoveChars(const std::string& in,
                 const char remove_chars[],
                 std::string* out);

bool RemoveChars(const std::wstring& in,
                 const wchar_t remove_chars[],
                 std::wstring* out);

/*
 @ brief:
    replace |find_with| with |replace_with| in |str|.
 @ params:
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
 @ brief:
    remove characters in |trim_chars| in a certain range of |in|.
 @ params:
    trim_chars[in] charaters that need to be removed from |in|. must be
    null-terminated.
 @ return:
    true, if having trimed.
    false, otherwise.
*/

bool TrimString(const std::string& in, const char trim_chars[], std::string* out);
bool TrimString(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out);

bool TrimLeadingStr(const std::string& in, const char trim_chars[], std::string* out);
bool TrimLeadingStr(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out);

bool TrimTailingStr(const std::string& in, const char trim_chars[], std::string* out);
bool TrimTailingStr(const std::wstring& in, const wchar_t trim_chars[], std::wstring* out);

}   // namespace StringUtil

}   // namespace KBase

#endif  // KBASE_STRINGS_STRING_UTIL_H_