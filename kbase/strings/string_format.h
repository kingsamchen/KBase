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
        : length_error(what)
    {}

    explicit StringPrintfDataLengthError(const char* what)
        : length_error(what)
    {}
};

class StringFormatSpecifierError : public std::invalid_argument {
public:
    explicit StringFormatSpecifierError(const std::string& what)
        : invalid_argument(what)
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
    using String = std::basic_string<CharT>;
    using Stream = std::basic_ostringstream<CharT>;
};

template<typename CharT>
struct Placeholder {
    using StrT = typename FmtStr<CharT>::String;

    unsigned long index = static_cast<unsigned long>(-1);
    unsigned long pos = static_cast<unsigned long>(-1);
    StrT format_specifier;
    StrT formatted;
};

template<typename CharT>
using PlaceholderList = std::vector<Placeholder<CharT>>;

// Return a simplified/analyzed format string, and store every specifiers into
// `placeholders`.
// Be aware of that elements in `placeholders` are sorted in the ascending order
// of index.

std::string AnalyzeFormatString(const char* fmt, PlaceholderList<char>* placeholders);
std::wstring AnalyzeFormatString(const wchar_t* fmt,
                                 PlaceholderList<wchar_t>* placeholders);

template<typename CharT>
typename FmtStr<CharT>::String StringFormatT(const typename FmtStr<CharT>::String& fmt,
                                             PlaceholderList<CharT>* placeholders,
                                             unsigned long arg_processing_index)
{
    return typename FmtStr<CharT>::String();
}

template<typename CharT, typename Arg, typename... Args>
typename FmtStr<CharT>::String StringFormatT(const typename FmtStr<CharT>::String& fmt,
                                             PlaceholderList<CharT>* placeholders,
                                             unsigned long arg_processing_index,
                                             Arg arg,
                                             Args... args)
{
    return StringFormatT(fmt, placeholders, arg_processing_index + 1, args...);
}

}   // namespace internal

// C#-like string format facility.

template<typename... Args>
std::string StringFormat(const char* fmt, Args... args)
{
    using namespace kbase::internal;

    PlaceholderList<char> placeholders;
    placeholders.reserve(sizeof...(args));
    auto simplified_fmt = AnalyzeFormatString(fmt, &placeholders);

    return internal::StringFormatT(simplified_fmt, &placeholders, 0, args...);
}

template<typename... Args>
std::wstring StringFormat(const wchar_t* fmt, Args... args)
{
    using namespace kbase::internal;

    PlaceholderList<wchar_t> placeholders;
    placeholders.reserve(sizeof...(args));
    auto simplified_fmt = AnalyzeFormatString(fmt, &placeholders);

    return internal::StringFormatT(simplified_fmt, &placeholders, 0, args...);
}

}   // namespace kbase

#endif  // KBASE_STRINGS_STRING_FORMAT_H_