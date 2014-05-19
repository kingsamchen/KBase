/*
 @ Kingsley Chen
*/

#include "kbase/strings/sys_string_encoding_conversions.h"

#include <windows.h>

#include <cassert>
#include <stdexcept>

#include "kbase/strings/string_util.h"

namespace kbase {

std::string SysWideToUTF8(const std::wstring& wide_str)
{
    return SysWideToMultiByte(wide_str, CodePage::UTF8);
}

std::wstring SysUTF8ToWide(const std::string& utf8_str)
{
    return SysMultiByteToWide(utf8_str, CodePage::UTF8);
}

std::string SysWideToNativeMB(const std::wstring& wide_str)
{
    return SysWideToMultiByte(wide_str, CodePage::Default_Code_Page);
}

std::wstring SysNativeMBToWide(const std::string& mb_str)
{
    return SysMultiByteToWide(mb_str, CodePage::Default_Code_Page);
}

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

std::wstring ASCIIToWide(const std::string& ascii_str)
{
    assert(IsStringASCII(ascii_str));
#if defined(NDEBUG)
    if (!IsStringASCII(ascii_str)) {
        throw std::invalid_argument("string contains non-ASCII characters!");
    }
#endif

    return std::wstring(ascii_str.cbegin(), ascii_str.cend());
}

std::string WideToASCII(const std::wstring& wide_str)
{
    assert(IsStringASCII(wide_str));
#if defined(NDEBUG)
    if (!IsStringASCII(wide_str)) {
        throw std::invalid_argument("string contains non-ASCII characters!");
    }
#endif

    return std::string(wide_str.cbegin(), wide_str.cend());
}

}   // namespace kbase