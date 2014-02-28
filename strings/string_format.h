/*
 @ Kingsley Chen
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_STRINGS_STRING_FORMAT_H_
#define KBASE_STRINGS_STRING_FORMAT_H_

#include <stdarg.h>

#include <string>

namespace KBase {

// all the following functions can throw an exception, if the size of the buffer
// that stores the formatted data exceeds the threshold.

std::string StringPrintf(const char* fmt, ...);
std::wstring StringPrintf(const wchar_t* fmt, ...);

const std::string& SStringPrintf(std::string* str, const char* fmt, ...);
const std::wstring& SStringPrintf(std::string* str, const wchar_t* fmt, ...);

void StringAppendF(std::string* str, const char* fmt, ...);
void StringAppendF(std::wstring* str, const wchar_t* fmt, ...);

}   // namespace KBase

#endif  // KBASE_STRINGS_STRING_FORMAT_H_