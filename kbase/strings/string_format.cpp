/*
 @ Kingsley Chen
*/

#include "kbase/strings/string_format.h"

#include <vector>

#include "kbase/scope_guard.h"

namespace kbase {

inline int vsnprintfT(char* buf, size_t buf_size, size_t count_to_write,
                      const char* fmt, va_list args)
{
    return vsnprintf_s(buf, buf_size, count_to_write, fmt, args);
}

inline int vsnprintfT(wchar_t* buf, size_t buf_size, size_t count_to_write,
                      const wchar_t* fmt, va_list args)
{
    return _vsnwprintf_s(buf, buf_size, count_to_write, fmt, args);
}

template<typename strT>
void StringAppendFT(strT* str, const typename strT::value_type* fmt, va_list ap)
{
    typedef typename strT::value_type charT;

    const int kDefaultCharCount = 1024;
    charT buf[kDefaultCharCount];

    int ret = vsnprintfT(buf, kDefaultCharCount, kDefaultCharCount - 1, fmt, ap);

    if (ret >= 0) {
        str->append(buf, ret);
        return;
    }

    // data is truncated.
    // adjust the buffer size until it fits

    const int kMaxAllowedCharCount = (1 << 25);
    int tentative_char_count = kDefaultCharCount;
    while (true) {
        tentative_char_count <<= 1;
        if (tentative_char_count > kMaxAllowedCharCount) {
            throw StringFormatDataLengthError("memory needed exceeds the threshold");
        }

        std::vector<charT> dynamic_buf(tentative_char_count);

        // vsnprintf-like functions on Windows don't change the |ap|
        // while their counterparts on Linux do.
        // if you use VS2013 or higher, or compilers that support C99
        // you alternatively can use |va_copy| to make a copy of |ap|
        // during each iteration.
        int ret = vsnprintfT(&dynamic_buf[0], tentative_char_count,
                             tentative_char_count - 1, fmt, ap);
        if (ret >= 0) {
            str->append(&dynamic_buf[0], ret);
            return;
        }
    }
}

void StringAppendF(std::string* str, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT([&] { va_end(args); });
    StringAppendFT(str, fmt, args);
}

void StringAppendF(std::wstring* str, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT([&] { va_end(args); });
    StringAppendFT(str, fmt, args);
}

std::string StringPrintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT([&] { va_end(args); });

    std::string str;
    StringAppendFT(&str, fmt, args);

    return str;
}

std::wstring StringPrintf(const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT([&] { va_end(args); });

    std::wstring str;
    StringAppendFT(&str, fmt, args);

    return str;
}

const std::string& SStringPrintf(std::string* str, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT([&] { va_end(args); });

    str->clear();
    StringAppendFT(str, fmt, args);

    return *str;
}

const std::wstring& SStringPrintf(std::wstring* str, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT([&] { va_end(args); });

    str->clear();
    StringAppendFT(str, fmt, args);

    return *str;
}

}   // namespace kbase