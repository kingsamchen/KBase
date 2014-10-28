/*
 @ Kingsley Chen
*/

#include "kbase\error_exception_util.h"

#include <Windows.h>

#include "kbase\strings\string_format.h"
#include "kbase\strings\sys_string_encoding_conversions.h"

namespace kbase {

LastError::LastError()
    : error_code_(GetLastError())
{}

unsigned long LastError::last_error_code() const
{
    return error_code_;
}

std::wstring LastError::GetVerboseMessage() const
{
    HLOCAL buffer = nullptr;
    DWORD lang_id = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
    DWORD text_length = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                      FORMAT_MESSAGE_FROM_SYSTEM |
                                      FORMAT_MESSAGE_IGNORE_INSERTS,
                                      nullptr,
                                      error_code_,
                                      lang_id,
                                      reinterpret_cast<LPTSTR>(&buffer),
                                      0,
                                      nullptr);

    // checks if network-related error
    if (text_length == 0) {
        HMODULE dll = ::LoadLibraryEx(L"netmsg.dll", nullptr,
                                      DONT_RESOLVE_DLL_REFERENCES);
        if (!dll) {
            text_length = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                          FORMAT_MESSAGE_IGNORE_INSERTS |
                                          FORMAT_MESSAGE_FROM_HMODULE,
                                          dll,
                                          error_code_,
                                          lang_id,
                                          reinterpret_cast<LPTSTR>(&buffer),
                                          0,
                                          nullptr);
            ::FreeLibrary(dll);
        }
    }

    // best efforts only...
    std::wstring message_text;
    if (text_length == 0 || !buffer) {
        return message_text;
    }

    // Removes the trailing \r\n.
    message_text.assign(static_cast<LPCTSTR>(LocalLock(buffer)), text_length - 2);
    ::LocalFree(buffer);

    return message_text;
}

Win32Exception::Win32Exception(unsigned long last_error,
                               const std::string& message)
    : runtime_error(message), error_code_(last_error)
{}

unsigned long Win32Exception::error_code() const
{
    return error_code_;
}

void ThrowLastErrorIfInternal(const char* file, int line, const char* fn_name,
                              bool expression, const std::string& user_message)
{
    if (expression) {
        LastError last_error;

        // Since GetVerboseMessage internally uses English as its displaying language,
        // it is safe here to call WideToASCII.
        std::string last_error_message = WideToASCII(last_error.GetVerboseMessage());
        std::string error_message = 
            StringPrintf("File: %s Line: %d Function: %s\n", file, line, fn_name);
        error_message += user_message + " (" + last_error_message + ")";
        
        throw Win32Exception(last_error.last_error_code(), error_message);
    }
}

}   // namespace kbase