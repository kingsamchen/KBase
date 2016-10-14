/*
 @ 0xCCCCCCCC
*/

#include "kbase/error_exception_util.h"

#include <ctime>

#include "kbase/scope_guard.h"
#include "kbase/stack_walker.h"

#if defined(OS_WIN)
#include <Windows.h>
#endif

#if defined(OS_WIN)
#include "kbase/minidump.h"
#endif

namespace {

#if defined(OS_WIN)

using kbase::Path;
using kbase::PathString;

Path g_minidump_dir_path;

// TODO: replace raw time with human-readable substitution.
PathString GenerateMiniDumpFileName()
{
    PathString file_name(L"crash_");
    file_name += std::to_wstring(time(nullptr));
    file_name += L".dmp";

    return file_name;
}

#endif  // OS_WIN

bool g_always_check_first_in_debug = true;

bool ShouldCheckFirst()
{
#if defined(NDEBUG)
    return false;
#else
    return g_always_check_first_in_debug;
#endif
}

}   // namespace

namespace kbase {

void AlwaysCheckFirstInDebug(bool always_check)
{
    g_always_check_first_in_debug = always_check;
}

void Guarantor::Require()
{
    switch (action_required_) {
        case EnsureAction::CHECK:
            Check();
            break;

        case EnsureAction::RAISE:
            Raise();
            break;

#if defined(OS_WIN)
          case EnsureAction::RAISE_WITH_DUMP:
            RaiseWithDump();
            break;
#endif

        default:
            Check();
            break;
    }
}

void Guarantor::Require(StringView msg)
{
    exception_desc_ << "Extra Message: " << msg << "\n";
    Require();
}

// TODO: Add portable DebugPresent() check and then DebugBreak() if a debugger is attached.
void Guarantor::Check()
{
    StackWalker callstack;
    callstack.DumpCallStack(exception_desc_);
    std::string description = exception_desc_.str();
#if defined(OS_WIN)
    std::wstring message = UTF8ToWide(description);
    MessageBoxW(nullptr, message.c_str(), L"Checking Failed", MB_OK | MB_TOPMOST | MB_ICONHAND);
    __debugbreak();
#else
    fwrite(description.data(), sizeof(char), description.length(), stderr);
    fflush(stderr);
    abort();
#endif
}

void Guarantor::Raise()
{
    if (ShouldCheckFirst()) {
        Check();
    }

    StackWalker callstack;
    callstack.DumpCallStack(exception_desc_);
    throw std::runtime_error(exception_desc_.str());
}

#if defined(OS_WIN)

void Guarantor::RaiseWithDump()
{
    if (ShouldCheckFirst()) {
        Check();
    }

    // If succeeded in creating the minidump, throws an exception with path attached;
    // Otherwise, throws a normal exception instead.
    auto dump_file_path = g_minidump_dir_path.AppendWith(GenerateMiniDumpFileName());
    bool created = CreateMiniDump(dump_file_path);
    if (created) {
        throw ExceptionWithMiniDump(dump_file_path, exception_desc_.str());
    }

    throw std::runtime_error(exception_desc_.str());
}

void SetMiniDumpDirectory(const Path& dump_dir)
{
    g_minidump_dir_path = dump_dir;
}

// -*- LastError implementation -*-

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
    ON_SCOPE_EXIT { if (buffer) LocalFree(buffer); };

    // Check if it's a network-related error.
    if (text_length == 0) {
        HMODULE dll = LoadLibraryExW(L"netmsg.dll", nullptr, DONT_RESOLVE_DLL_REFERENCES);
        if (dll) {
            ON_SCOPE_EXIT { FreeLibrary(dll); };
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

#endif  // OS_WIN

}   // namespace kbase