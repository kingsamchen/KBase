/*
 @ Kingsley Chen
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_STRINGS_SYS_STRING_ENCODING_CONVERSIONS_H_
#define KBASE_STRINGS_SYS_STRING_ENCODING_CONVERSIONS_H_

#include <string>

namespace KBase {

enum class CodePage {
    Default_Code_Page = 0,
    GB_Chinese_Encoding = 54936,
    UTF8 = 65001
};

std::string SysWideToUTF8(const std::wstring& str);
std::wstring SysUTF8ToWide(const std::string& str);

std::string SysWideToNativeMB(const std::wstring& str);
std::wstring SysNativeMBToWide(const std::string& str);

std::wstring SysMultiByteToWide(const std::string& mb_str, CodePage code_page);
std::string SysWideToMultiByte(const std::wstring& wide_str, CodePage code_page);

}   // namespace KBase

#endif  // KBASE_STRINGS_SYS_STRING_ENCODING_CONVERSIONS_H_