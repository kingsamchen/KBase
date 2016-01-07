/*
 @ 0xCCCCCCCC
*/

#include "kbase/error_exception_util.h"

#include <Windows.h>

#include "kbase/scope_guard.h"

namespace {

bool g_always_enable_check_in_debug = true;

inline bool ShouldCheckFirst()
{
    return ACTION_IS_ON(CHECK) && g_always_enable_check_in_debug;
}

}   // namespace

namespace kbase {

void Guarantor::Require() const
{
    switch (action_required_) {
        case EnsureAction::CHECK:
            Check();
            break;

        case EnsureAction::RAISE:
            Raise();
            break;

        case EnsureAction::RAISE_WITH_DUMP:
            RaiseWithDump();
            break;
    }
}

void Guarantor::Require(const std::string msg)
{
    exception_desc_ << "Extra Message: " << msg << "\n";
    Require();
}

void Guarantor::Check() const
{
    std::wstring message = SysUTF8ToWide(exception_desc_.str());
    MessageBoxW(nullptr, message.c_str(), L"Checking Failed", MB_OK | MB_TOPMOST | MB_ICONHAND);
    __debugbreak();
}

void Guarantor::Raise() const
{
    if (ShouldCheckFirst()) {
        Check();
    }

    throw std::runtime_error(exception_desc_.str());
}

void Guarantor::RaiseWithDump() const
{
    if (ShouldCheckFirst()) {
        Check();
    }

    // TODO: dump and throw.
}

void EnableAlwaysCheckForEnsureInDebug(bool always_check)
{
    g_always_enable_check_in_debug = always_check;
}

LastError::LastError()
    : error_code_(GetLastError())
{}

unsigned long LastError::error_code() const
{
    return error_code_;
}

std::wstring LastError::GetDescriptiveMessage() const
{
    HLOCAL buffer = nullptr;
    DWORD lang_id = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
    DWORD text_length = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                       FORMAT_MESSAGE_FROM_SYSTEM |
                                       FORMAT_MESSAGE_IGNORE_INSERTS,
                                       nullptr,
                                       error_code_,
                                       lang_id,
                                       reinterpret_cast<LPTSTR>(&buffer),
                                       0,
                                       nullptr);
    ON_SCOPE_EXIT([&buffer] { if (buffer) LocalFree(buffer); });

    // Check if it's a network-related error.
    if (text_length == 0) {
        HMODULE dll = LoadLibraryExW(L"netmsg.dll", nullptr, DONT_RESOLVE_DLL_REFERENCES);
        if (dll) {
            ON_SCOPE_EXIT([&dll] { FreeLibrary(dll); });
            text_length = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                         FORMAT_MESSAGE_IGNORE_INSERTS |
                                         FORMAT_MESSAGE_FROM_HMODULE,
                                         dll,
                                         error_code_,
                                         lang_id,
                                         reinterpret_cast<LPTSTR>(&buffer),
                                         0,
                                         nullptr);
        }
    }

    // Best efforts only...
    std::wstring message_text;
    if (text_length == 0 || !buffer) {
        return message_text;
    }

    // Remove the trailing \r\n.
    message_text.assign(static_cast<LPCTSTR>(LocalLock(buffer)), text_length - 2);

    return message_text;
}

std::ostream& operator<<(std::ostream& os, const LastError& last_error)
{
    std::string error_message = WideToASCII(last_error.GetDescriptiveMessage());
    os << last_error.error_code()
       << " (" + error_message + ")";

    return os;
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
        std::string last_error_message = WideToASCII(last_error.GetDescriptiveMessage());
        std::string error_message =
            StringPrintf("File: %s Line: %d Function: %s\n", file, line, fn_name);
        error_message += user_message + " (" + last_error_message + ")";

        throw Win32Exception(last_error.error_code(), error_message);
    }
}

}   // namespace kbase