/*
 @ Kingsley Chen
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_STRINGS_STRING_FORMAT_H_
#define KBASE_STRINGS_STRING_FORMAT_H_

#include <stdarg.h>

#include <stdexcept>
#include <string>

namespace kbase {

// All Printf-series functions may throw an exception, if the size of the buffer
// that stores the formatted data exceeds the threshold.

class StringFormatDataLengthError : public std::length_error {
public:
    explicit StringFormatDataLengthError(const std::string& what)
        : std::length_error(what)
    {}

    explicit StringFormatDataLengthError(const char* what)
        : std::length_error(what)
    {}
};

// Return a string with given format.

std::string StringPrintf(const char* fmt, ...);
std::wstring StringPrintf(const wchar_t* fmt, ...);

// Modify a string with given format and return the string.

const std::string& StringPrintf(std::string* str, const char* fmt, ...);
const std::wstring& StringPrintf(std::string* str, const wchar_t* fmt, ...);

void StringAppendPrintf(std::string* str, const char* fmt, ...);
void StringAppendPrintf(std::wstring* str, const wchar_t* fmt, ...);

}   // namespace kbase

#endif  // KBASE_STRINGS_STRING_FORMAT_H_