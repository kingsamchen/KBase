/*
 @ Kingsley Chen
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_STRINGS_STRING_FORMAT_H_
#define KBASE_STRINGS_STRING_FORMAT_H_

#include <stdarg.h>

#include <algorithm>
#include <cassert>
#include <iomanip>
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

    // For std::equal_range.
    struct RangeCmp {
        bool operator()(unsigned long index, const Placeholder& rhs) const
        {
            return index < rhs.index;
        }

        bool operator()(const Placeholder& lhs, unsigned long index) const
        {
            return lhs.index < index;
        }

        // HACK: VS2013 needs this function for a paranoid pre-sorted check
        // only in debug mode.
#if _DEBUG
        bool operator()(const Placeholder& lhs, const Placeholder& rhs)
        {
            return lhs.index < rhs.index;
        }
#endif
    };
};

template<typename CharT>
using PlaceholderList = std::vector<Placeholder<CharT>>;

// This wrapper provides a more semantic validation.
inline void EnsureFormatSpecifier(bool expr)
{
    if (!expr) {
        throw StringFormatSpecifierError("Format string is not valid");
    }
}

// Return a simplified/analyzed format string, and store every specifiers into
// `placeholders`.
// Be aware of that elements in `placeholders` are sorted in the ascending order
// of index.

std::string AnalyzeFormatString(const char* fmt, PlaceholderList<char>* placeholders);
std::wstring AnalyzeFormatString(const wchar_t* fmt,
                                 PlaceholderList<wchar_t>* placeholders);

enum class SpecifierCategory {
    NONE = 0,
    PADDING_ALIGN,
    SIGN,
    WIDTH,
    PRECISION,
    TYPE
};

inline bool IsDigit(char ch)
{
    return isdigit(ch) != 0;
}

inline bool IsDigit(wchar_t ch)
{
    return iswdigit(ch) != 0;
}

inline unsigned long StrToUL(const char* str, char** end_ptr)
{
    return strtoul(str, end_ptr, 10);
}

inline unsigned long StrToUL(const wchar_t* str, wchar_t** end_ptr)
{
    return wcstoul(str, end_ptr, 10);
}

inline bool IsTypeSpecifier(wchar_t ch)
{
    return ch == L'b' || ch == 'x' || ch == 'X' || ch == 'o' || ch == 'e' || ch == 'E';
}

template<typename CharT>
SpecifierCategory GuessNextSpecCategory(const CharT* spec)
{
    // Maybe we have finished parsing.
    if (*spec == '\0') {
        return SpecifierCategory::NONE;
    }

    if (*(spec + 1) == '<' || *(spec + 1) == '>') {
        return SpecifierCategory::PADDING_ALIGN;
    }

    if (*spec == '+') {
        return SpecifierCategory::SIGN;
    }

    if (IsDigit(*spec)) {
        return SpecifierCategory::WIDTH;
    }

    if (*spec == '.') {
        return SpecifierCategory::PRECISION;
    }

    if (IsTypeSpecifier(*spec)) {
        return SpecifierCategory::TYPE;
    }

    // Not reached.
    assert(false);
    EnsureFormatSpecifier(false);
    return SpecifierCategory::NONE;
}

template<typename CharT>
void ApplyPaddingAlignFormat(const CharT* spec,
                             SpecifierCategory last_spec_type,
                             typename FmtStr<CharT>::Stream* stream,
                             const CharT** spec_end)
{
    EnsureFormatSpecifier(last_spec_type < SpecifierCategory::PADDING_ALIGN);

    typename FmtStr<CharT>::Stream& os = *stream;

    CharT fill_ch = *spec++;
    os << std::setfill(fill_ch);
    if (*spec == '<') {
        os << std::left;
    } else {
        os << std::right;
    }

    *spec_end = spec + 1;
}

template<typename CharT>
void ApplySignFormat(const CharT* spec,
                     SpecifierCategory last_spec_type,
                     typename FmtStr<CharT>::Stream* stream,
                     const CharT** spec_end)
{
    EnsureFormatSpecifier(last_spec_type < SpecifierCategory::SIGN);

    typename FmtStr<CharT>::Stream& os = *stream;

    os << std::showpos;

    *spec_end = spec + 1;
}

template<typename CharT>
void ApplyWidthFormat(const CharT* spec,
                      SpecifierCategory last_spec_type,
                      typename FmtStr<CharT>::Stream* stream,
                      const CharT** spec_end)
{
    EnsureFormatSpecifier(last_spec_type < SpecifierCategory::WIDTH);

    typename FmtStr<CharT>::Stream& os = *stream;

    CharT* digit_end = nullptr;
    auto width = StrToUL(spec, &digit_end);
    os << std::setw(width);

    *spec_end = digit_end;
}

template<typename CharT>
void ApplyPrecisionFormat(const CharT* spec,
                          SpecifierCategory last_spec_type,
                          typename FmtStr<CharT>::Stream* stream,
                          const CharT** spec_end)
{
    EnsureFormatSpecifier(last_spec_type < SpecifierCategory::PRECISION);

    typename FmtStr<CharT>::Stream& os = *stream;

    CharT* digit_end = nullptr;
    auto precision_size = StrToUL(spec + 1, &digit_end);
    os << std::setprecision(precision_size);

    *spec_end = digit_end;
}

template<typename CharT>
void ApplyTypeFormat(const CharT* spec,
                     SpecifierCategory last_spec_type,
                     typename FmtStr<CharT>::Stream* stream,
                     const CharT** spec_end)
{
    EnsureFormatSpecifier(last_spec_type < SpecifierCategory::TYPE);

    typename FmtStr<CharT>::Stream& os = *stream;

    CharT type_mark = *spec;
    switch (type_mark) {
        case 'b':
            os << std::boolalpha;
            break;

        case 'x':
            os << std::hex;
            break;

        case 'X':
            os << std::hex << std::uppercase;
            break;

        case 'o':
            os << std::oct;
            break;

        case 'e':
            os << std::scientific;
            break;

        case 'E':
            os << std::scientific << std::uppercase;
            break;

        default:
            // Not reached.
            assert(false);
            break;
    }

    *spec_end = spec + 1;
}

template<typename CharT, typename Arg>
void FormatWithSpecifier(const Arg& arg,
                         const typename FmtStr<CharT>::String& specifier,
                         typename FmtStr<CharT>::Stream* stream,
                         typename FmtStr<CharT>::String* formatted)
{
    if (specifier.empty()) {
        (*stream) << arg;
        *formatted = stream->str();
        return;
    }

    auto spec = specifier.data();
    auto last_spec_type = SpecifierCategory::NONE;
    auto next_spec_type = SpecifierCategory::NONE;
    while ((next_spec_type = GuessNextSpecCategory(spec)) != SpecifierCategory::NONE) {
        switch (next_spec_type) {
            case SpecifierCategory::PADDING_ALIGN:
                ApplyPaddingAlignFormat(spec, last_spec_type, stream, &spec);
                last_spec_type = SpecifierCategory::PADDING_ALIGN;
                break;

            case SpecifierCategory::SIGN:
                ApplySignFormat(spec, last_spec_type, stream, &spec);
                last_spec_type = SpecifierCategory::SIGN;
                break;

            case SpecifierCategory::WIDTH:
                ApplyWidthFormat(spec, last_spec_type, stream, &spec);
                last_spec_type = SpecifierCategory::WIDTH;
                break;

            case SpecifierCategory::PRECISION:
                ApplyPrecisionFormat(spec, last_spec_type, stream, &spec);
                last_spec_type = SpecifierCategory::PRECISION;
                break;

            case SpecifierCategory::TYPE:
                ApplyTypeFormat(spec, last_spec_type, stream, &spec);
                last_spec_type = SpecifierCategory::TYPE;
                break;

            // Not reached.
            default:
                assert(false);
                EnsureFormatSpecifier(false);
                break;
        }
    }

    (*stream) << arg;
    *formatted = stream->str();
}

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
                                             const Arg& arg,
                                             const Args&... args)
{
    auto ph_range = std::equal_range(placeholders->begin(),
                                     placeholders->end(),
                                     arg_processing_index,
                                     typename Placeholder<CharT>::RangeCmp());
    typename FmtStr<CharT>::Stream output;
    auto default_fmt_flags = output.flags();
    for (auto it = ph_range.first; it != ph_range.second; ++it) {
        FormatWithSpecifier<CharT, Arg>(arg,
                                        it->format_specifier,
                                        &output,
                                        &it->formatted);
        // Reset stream.
        output.str(typename FmtStr<CharT>::String());
        output.clear();
        output.flags(default_fmt_flags);
    }

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