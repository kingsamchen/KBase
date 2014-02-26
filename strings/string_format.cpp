/*
 @ Kingsley Chen
*/

#include "string_format.h"

namespace KBase {

inline int vsnprintfT(char* buf, size_t buf_size, size_t count_to_write, const char* fmt, va_list args)
{
    return vsnprintf_s(buf, buf_size, count_to_write, fmt, args);
}

inline int vsnprintfT(wchar_t* buf, size_t buf_size, size_t count_to_write, const wchar_t* fmt, va_list args)
{
    return _vsnwprintf_s(buf, buf_size, count_to_write, fmt, args);
}

template<typename strT>
void StringAppendFT(strT* str, const typename strT::value_type* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    const int kDefaultBufSize  = 1024;
    strT::value_type buf[kDefaultBufSize];

    int ret = vsnprintfT(buf, kDefaultBufSize, kDefaultBufSize - 1, fmt, args);

    if (ret != -1) {
        str->append(buf);
        va_end(args)
        return;
    }

    // data is truncated.
    // attempt to adjust the buffer size

    int buf_size = kDefaultBufSize;
    do {

    } while ();
}

}   // namespace KBase