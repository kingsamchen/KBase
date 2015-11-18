/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SYS_STRING_ENCODING_CONVERSIONS_H_
#define KBASE_SYS_STRING_ENCODING_CONVERSIONS_H_

#include <string>

#include "kbase\string_piece.h"

namespace kbase {

// defined according to MSDN
enum class CodePage {
    Default_Code_Page = 0,
    GB_Chinese_Encoding = 54936,
    UTF8 = 65001
};

std::string SysWideToUTF8(const std::wstring& wide_str);
std::wstring SysUTF8ToWide(const std::string& utf8_str);

std::string SysWideToNativeMB(const std::wstring& wide_str);
std::wstring SysNativeMBToWide(const std::string& mb_str);

std::wstring SysMultiByteToWide(const std::string& mb_str, CodePage code_page);
std::string SysWideToMultiByte(const std::wstring& wide_str, CodePage code_page);

// If the string being converted contains non-ASCII characters, functions throw
// an invalid_argument exception.
std::wstring ASCIIToWide(const StringPiece& ascii_str);
std::string WideToASCII(const WStringPiece& wide_str);

}   // namespace kbase

#endif  // KBASE_SYS_STRING_ENCODING_CONVERSIONS_H_