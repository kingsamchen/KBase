/*
 @ Kingsley Chen
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef STRINGS_STRING_FORMAT_H_
#define STRINGS_STRING_FORMAT_H_

#include <stdarg.h>

#include <string>

namespace KBase {

std::string StringPrintf(const char* fmt, ...);
std::wstring StringPrintf(const wchar_t* fmt, ...);

const std::string& SStringPrintf(std::string* str, const char* fmt, ...);
const std::wstring& SStringPrintf(std::string* str, const wchar_t* fmt, ...);

void StringAppendF(std::string* str, const char* fmt, ...);
void StringAppendF(std::wstring* str, const wchar_t* fmt, ...);

}   // namespace KBase

#endif  // STRINGS_STRING_FORMAT_H_