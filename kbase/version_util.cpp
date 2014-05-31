/*
 @ Kingsley Chen
*/

#include "kbase/version_util.h"

#include <Windows.h>

namespace kbase {

typedef OSInfo::WOW64Status WOW64Status;

OSInfo::OSInfo()
 : version_(VERSION_PRE_XP), architecture_(UNKNOWN_ARCHITECTURE),
   wow64_status_(GetWOW64StatusForProcess(GetCurrentProcess()))
{
}

OSInfo::~OSInfo()
{}

// static
WOW64Status OSInfo::GetWOW64StatusForProcess(HANDLE process)
{
    typedef BOOL (WINAPI *IsWow64ProcessFunc) (HANDLE, BOOL*);
    
    IsWow64ProcessFunc is_wow64_process = reinterpret_cast<IsWow64ProcessFunc>(
        GetProcAddress(GetModuleHandle(L"kernel32.dll"), "IsWow64Process"));

    if (!is_wow64_process) {
        return WOW64Status::WOW64_DISABLED;
    }

    BOOL is_wow64 = FALSE;
    if (!is_wow64_process(process, &is_wow64)) {
        return WOW64Status::WOW64_UNKNOWN;
    }

    return is_wow64 ? WOW64Status::WOW64_ENABLED : WOW64Status::WOW64_DISABLED;
}

}   // namespace kbase