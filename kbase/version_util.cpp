/*
 @ Kingsley Chen
*/

#include "kbase/version_util.h"

#include <map>

#include <Windows.h>
#include <VersionHelpers.h>

namespace kbase {

typedef OSInfo::WOW64Status WOW64Status;

namespace {

struct VersionNumber {
    WORD major_version;
    WORD minor_version;
    WORD service_pack_major;
};

const std::map<Version, VersionNumber> version_name_to_number = {
    {Version::WIN_XP, {5, 1, 0}},
    {Version::WIN_XP_SP3, {5, 1, 3}},
    {Version::WIN_SERVER_2003, {5, 2, 0}},
    {Version::WIN_VISTA, {6, 0, 0}},
    {Version::WIN_7, {6, 1, 0}},
    {Version::WIN_8, {6, 2, 0}},
    {Version::WIN_8_1, {6, 3, 0}}
};

}   // namespace

// static
OSInfo* OSInfo::GetInstance()
{
    static OSInfo* info = nullptr;
    if (!info) {
        OSInfo* new_info = new OSInfo();
        if (InterlockedCompareExchangePointer(
            reinterpret_cast<PVOID*>(&info), new_info, nullptr)) {
            delete new_info;
        }
    }

    return info;
}

OSInfo::OSInfo()
 : architecture_(UNKNOWN_ARCHITECTURE), 
   wow64_status_(GetWOW64StatusForProcess(GetCurrentProcess())),
   is_server_(IsWindowsServer())
{
    SYSTEM_INFO system_info = {0};
    GetNativeSystemInfo(&system_info);

    switch (system_info.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_INTEL:
            architecture_ = SystemArchitecture::X86_ARCHITECTURE;
            break;
        case PROCESSOR_ARCHITECTURE_AMD64:
            architecture_ = SystemArchitecture::X64_ARCHITECTURE;
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
            architecture_ = SystemArchitecture::IA64_ARCHITECTURE;
            break;
    }

    processors_ = system_info.dwNumberOfProcessors;
    allocation_granularity_ = system_info.dwAllocationGranularity;
}

OSInfo::~OSInfo()
{}

bool OSInfo::IsVersionOrGreater(Version version)
{
    const VersionNumber& version_number = version_name_to_number.at(version);
    bool ret = IsWindowsVersionOrGreater(version_number.major_version,
                                         version_number.minor_version,
                                         version_number.service_pack_major);
    
    return ret;
}

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