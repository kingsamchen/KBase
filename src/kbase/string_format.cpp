/*
 @ 0xCCCCCCCC
*/

#include "kbase/string_format.h"

#include "kbase/error_exception_util.h"
#include "kbase/scope_guard.h"

namespace {

using kbase::StringFormatSpecifierError;
using kbase::internal::FmtStr;
using kbase::internal::Placeholder;
using kbase::internal::PlaceholderList;
using kbase::internal::IsDigit;
using kbase::internal::StrToUL;
using kbase::internal::EnsureFormatSpecifier;

enum class FormatStringParseState {
    IN_TEXT,
    IN_FORMAT
};

const char kEscapeBegin = '{';
const char kEscapeEnd = '}';
const char kSpecifierDelimeter = ':';
const char kPlaceholderChar = '@';

void AppendPrintfT(std::string& str, char* buf, size_t max_count_including_null, const char* fmt,
                   va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    int real_size = vsnprintf(buf, max_count_including_null, fmt, args_copy);
    va_end(args_copy);
    if (real_size < max_count_including_null) {
        // vsnprintf() guarantees the resulting string will be terminated with a null-terminator.
        str.append(buf);
        return;
    }

    std::vector<char> backup_buf(real_size + 1);
    va_copy(args_copy, args);
    vsnprintf(backup_buf.data(), backup_buf.size(), fmt, args_copy);
    va_end(args_copy);
    str.append(backup_buf.data());
}

void AppendPrintfT(std::wstring& str, wchar_t* buf, size_t max_count_including_null,
                   const wchar_t* fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    int rv = vswprintf(buf, max_count_including_null, fmt, args_copy);
    va_end(args_copy);
    if (rv >= 0) {
        str.append(buf, rv);
        return;
    }

    constexpr size_t kMaxAllowed = 16U * 1024 * 1024;
    size_t tentative_count = max_count_including_null;
    std::vector<wchar_t> backup_buf;
    while (true) {
        tentative_count <<= 1;
        ENSURE(RAISE, tentative_count <= kMaxAllowed)(tentative_count)(kMaxAllowed).Require();
        backup_buf.resize(tentative_count);
        va_copy(args_copy, args);
        rv = vswprintf(backup_buf.data(), backup_buf.size(), fmt, args_copy);
        va_end(args_copy);
        if (rv > 0) {
            str.append(backup_buf.data(), rv);
            break;
        }
    }
}

template<typename CharT>
inline unsigned long ExtractPlaceholderIndex(const CharT* first_digit, CharT** last_digit)
{
    auto index = StrToUL(first_digit, last_digit);
    --*last_digit;

    return index;
}

template<typename CharT>
typename FmtStr<CharT>::String AnalyzeFormatStringT(const CharT* fmt,
                                                    PlaceholderList<CharT>* placeholders)
{
    const size_t kInitialCapacity = 32;
    typename FmtStr<CharT>::String analyzed_fmt;
    analyzed_fmt.reserve(kInitialCapacity);

    placeholders->clear();
    Placeholder<CharT> placeholder;

    auto state = FormatStringParseState::IN_TEXT;
    for (auto ptr = fmt; *ptr != '\0'; ++ptr) {
        if (*ptr == kEscapeBegin) {
            // `{` is an invalid token for format-state.
            EnsureFormatSpecifier(state != FormatStringParseState::IN_FORMAT);

            if (*(ptr + 1) == kEscapeBegin) {
                // Use `{{` to represent literal `{`.
                analyzed_fmt += kEscapeBegin;
                ++ptr;
            } else if (IsDigit(*(ptr + 1))) {
                CharT* last_digit;
                placeholder.index = ExtractPlaceholderIndex(ptr + 1, &last_digit);
                ptr = last_digit;
                EnsureFormatSpecifier(*(ptr + 1) == kEscapeEnd ||
                                      *(ptr + 1) == kSpecifierDelimeter);
                if (*(ptr + 1) == kSpecifierDelimeter) {
                    ++ptr;
                }

                // Get into format-state.
                state = FormatStringParseState::IN_FORMAT;
            } else {
                throw StringFormatSpecifierError("Format string is not valid");
            }
        } else if (*ptr == kEscapeEnd) {
            if (state == FormatStringParseState::IN_TEXT) {
                EnsureFormatSpecifier(*(ptr + 1) == kEscapeEnd);
                analyzed_fmt += kEscapeEnd;
                ++ptr;
            } else {
                placeholder.pos = analyzed_fmt.length();
                analyzed_fmt += '@';
                placeholders->push_back(placeholder);
                placeholder.format_specifier.clear();

                // Now we turn back to text-state.
                state = FormatStringParseState::IN_TEXT;
            }
        } else {
            if (state == FormatStringParseState::IN_TEXT) {
                analyzed_fmt += *ptr;
            } else {
                placeholder.format_specifier += *ptr;
            }
        }
    }

    EnsureFormatSpecifier(state == FormatStringParseState::IN_TEXT);

    std::sort(std::begin(*placeholders), std::end(*placeholders),
              [](const Placeholder<CharT>& lhs, const Placeholder<CharT>& rhs) {
        return lhs.index < rhs.index;
    });

    return analyzed_fmt;
}

} // namespace

namespace kbase {

template<typename StrT>
void StringAppendPrintfT(StrT& str, const typename StrT::value_type* fmt, va_list args)
{
    using CharT = typename StrT::value_type;

    constexpr size_t kDefaultCount = 1024U;
    CharT buf[kDefaultCount];

    AppendPrintfT(str, buf, kDefaultCount, fmt, args);
}

void StringAppendPrintf(std::string& str, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    StringAppendPrintfT(str, fmt, args);
}

void StringAppendPrintf(std::wstring& str, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    StringAppendPrintfT(str, fmt, args);
}

std::string StringPrintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    std::string str;
    StringAppendPrintfT(str, fmt, args);

    return str;
}

std::wstring StringPrintf(const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    std::wstring str;
    StringAppendPrintfT(str, fmt, args);

    return str;
}

void StringPrintf(std::string& str, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    str.clear();
    StringAppendPrintfT(str, fmt, args);
}

void StringPrintf(std::wstring& str, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    str.clear();
    StringAppendPrintfT(str, fmt, args);
}

namespace internal {

std::string AnalyzeFormatString(const char* fmt, PlaceholderList<char>* placeholders)
{
    return AnalyzeFormatStringT(fmt, placeholders);
}

std::wstring AnalyzeFormatString(const wchar_t* fmt,
                                 PlaceholderList<wchar_t>* placeholders)
{
    return AnalyzeFormatStringT(fmt, placeholders);
}

}   // namespace internal

}   // namespace kbase