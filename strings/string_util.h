
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_STRINGS_STRING_UTIL_H_
#define KBASE_STRINGS_STRING_UTIL_H_ 

#include <string>

namespace KBase {

namespace StringUtil {

class StringPiece;
class WStringPiece;

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



void ReplaceSubstr(std::string* str,
                   const std::string& find_with,
                   const std::string& replace_with,
                   std::string::size_type pos = 0);

void ReplaceSubstr(std::wstring* str,
                   const std::wstring& find_with,
                   const std::wstring& replace_with,
                   std::wstring::size_type pos = 0);

void ReplaceFirstSubStr();

}   // namespace StringUtil

}   // namespace KBase

#endif  // KBASE_STRINGS_STRING_UTIL_H_