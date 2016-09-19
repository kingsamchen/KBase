/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_STRING_FORMAT_H_
#define KBASE_STRING_FORMAT_H_

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

namespace kbase {

class StringFormatSpecifierError : public std::invalid_argument {
public:
    explicit StringFormatSpecifierError(const std::string& what)
        : invalid_argument(what)
    {}

    explicit StringFormatSpecifierError(const char* what)
        : invalid_argument(what)
    {}
};

// All Printf-series functions may throw an exception, if the size of the buffer that stores
// the formatted data exceeds the threshold.

// Return a string in given format.

std::string StringPrintf(const char* fmt, ...);
std::wstring StringPrintf(const wchar_t* fmt, ...);

// Modify a string in given format and return the string.

void StringPrintf(std::string& str, const char* fmt, ...);
void StringPrintf(std::string& str, const wchar_t* fmt, ...);

// Append a string in given format.

void StringAppendPrintf(std::string& str, const char* fmt, ...);
void StringAppendPrintf(std::wstring& str, const wchar_t* fmt, ...);

namespace internal {

template<typename CharT>
struct FmtStr {
    using String = std::basic_string<CharT>;
    using Stream = std::basic_ostringstream<CharT>;
};

template<typename CharT>
struct Placeholder {
    using StrT = typename FmtStr<CharT>::String;

    size_t index = static_cast<size_t>(-1);
    size_t pos = static_cast<size_t>(-1);
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
    os << std::fixed << std::setprecision(precision_size);

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
                                             unsigned long arg_count)
{
    EnsureFormatSpecifier(std::all_of(placeholders->begin(), placeholders->end(),
                                      [arg_count](const Placeholder<CharT>& ph) {
        return ph.index < arg_count;
    }));

    auto formatted_str(fmt);

    // Sort in the decreasing order of pos, since we are going to expand
    // `formatted_str` from right to left.
    std::sort(placeholders->begin(), placeholders->end(),
              [](const Placeholder<CharT>& lhs, const Placeholder<CharT>& rhs) {
        return lhs.pos > rhs.pos;
    });

    for (const auto& ph : *placeholders) {
        formatted_str.replace(ph.pos, 1, ph.formatted);
    }

    return formatted_str;
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
// For each format-specifier in `fmt`, it can be in the form of
// {index[:[fill|align]|sign|width|.precision|type]}
// index := the 0-based index number of specifier.
// fill := any single character other than `{` and `}` for filling the padding.
// align := `<` for left-alignemnt with fill character and `>` for right-alignment.
// sign := +, prepend `+` with the positive number.
// width := the width of the field.
// .precision := floating-point precision.
// type := can be one of [b, x, X, o, e, E].
// Specifier marks `fill` and `align` **must** be in presence together.
// Although all of these specifier marks are optional, their relative orders, if any
// present, do matter; otherwise, a StringFormatSpecifierError exception would be raised.
// Also, if a specifier mark has no effect on its corresponding argument, this specifier
// mark is simply ignored, and no exception would be raised.

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

#endif  // KBASE_STRING_FORMAT_H_