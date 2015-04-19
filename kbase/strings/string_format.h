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
#include <vector>

namespace kbase {

// All Printf-series functions may throw an exception, if the size of the buffer
// that stores the formatted data exceeds the threshold.

class StringPrintfDataLengthError : public std::length_error {
public:
    explicit StringPrintfDataLengthError(const std::string& what)
        : std::length_error(what)
    {}

    explicit StringPrintfDataLengthError(const char* what)
        : std::length_error(what)
    {}
};

class StringFormatSpecifierError : public std::invalid_argument {
public:
    explicit StringFormatSpecifierError(const std::string& what)
        : std::invalid_argument(what)
    {}

    explicit StringFormatSpecifierError(const char* what)
        : invalid_argument(what)
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

namespace internal {

template<typename CharT>
struct FmtStr {
    using String = std::conditional_t<std::is_same<CharT, char>::value,
                                      std::string, std::wstring>;
    using Stream = std::conditional_t<std::is_same<CharT, char>::value,
                                      std::ostringstream, std::wostringstream>;
};

template<typename CharT>
typename FmtStr<CharT>::String StringFormatInternal(const CharT* fmt)
{
    return typename FmtStr<CharT>::String();
}

template<typename CharT, typename Arg, typename... Args>
typename FmtStr<CharT>::String StringFormatInternal(const CharT* fmt, Arg arg, Args ... args)
{
    return StringFormatInternal(fmt, args...);
}

}   // namespace internal

// C#-like string format facility.

template<typename... Args>
std::string StringFormat(const char* fmt, Args... args)
{
    return internal::StringFormatInternal(fmt, args...);
}

template<typename... Args>
std::wstring StringFormat(const wchar_t* fmt, Args... args)
{
    return internal::StringFormatInternal(fmt, args...);
}

}   // namespace kbase

#endif  // KBASE_STRINGS_STRING_FORMAT_H_