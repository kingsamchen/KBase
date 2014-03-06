/*
 @ Kingsley Chen
*/

#include "sys_string_encoding_conversions.h"

#include <vector>

#include <windows.h>

#include "string_util.h"

namespace KBase {

std::wstring SysMultiByteToWide(const std::string& mb_str, CodePage code_page)
{
    if (mb_str.empty())
        return std::wstring();

    // |char_count| does not include null-terminate.
    int mb_str_length = static_cast<int>(mb_str.length());
    int char_count = MultiByteToWideChar(static_cast<unsigned>(code_page), 0,
                                         mb_str.data(), mb_str_length,
                                         nullptr, 0);
    if (char_count == 0)
        return std::wstring();

    std::wstring wide_str;
    wchar_t* ptr = WriteInto(&wide_str, char_count + 1);
    MultiByteToWideChar(static_cast<unsigned>(code_page), 0, mb_str.data(), mb_str_length, ptr, char_count);

    return wide_str;
}

std::string SysWideToMultiByte(const std::wstring& wide_str, CodePage code_page)
{
    if (wide_str.empty())
        return std::string();

    int wide_str_length = static_cast<int>(wide_str.length());
    int char_count = WideCharToMultiByte(static_cast<int>(code_page), 0,
                                         wide_str.data(), wide_str_length,
                                         nullptr, 0,
                                         nullptr, nullptr);
    if (char_count == 0)
        return std::string();

    std::string mb_str;
    char* ptr = WriteInto(&mb_str, char_count + 1);
    WideCharToMultiByte(static_cast<int>(code_page), 0, wide_str.data(), wide_str_length,
                        ptr, char_count, nullptr, nullptr);

    return mb_str;
}

}   // namespace KBase