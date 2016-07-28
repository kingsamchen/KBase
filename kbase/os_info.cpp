/*
 @ 0xCCCCCCCC
*/

#include "kbase/os_info.h"

#include <map>

#include <Windows.h>
#include <VersionHelpers.h>

#include "kbase/registry.h"

namespace {

using kbase::SystemVersion;
using kbase::WOW64Status;

using VersionNumber = kbase::OSInfo::VersionNumber;

const std::map<SystemVersion, VersionNumber> version_name_to_number = {
    { SystemVersion::WIN_VISTA, { 6, 0 } },
    { SystemVersion::WIN_7, { 6, 1 } },
    { SystemVersion::WIN_8, { 6, 2 } },
    { SystemVersion::WIN_8_1, { 6, 3 } },
    { SystemVersion::WIN_10, { 10, 0 } }
};

std::wstring GetProcessorModelName()
{
    const wchar_t kKeyName[] = L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";

    kbase::RegKey key(HKEY_LOCAL_MACHINE);
    key.OpenKey(kKeyName, KEY_READ);
    std::wstring processor_model_name;
    key.ReadValue(L"ProcessorNameString", &processor_model_name);

    return processor_model_name;
}

void GetSystemVersion(VersionNumber& version_number) noexcept
{
    constexpr NTSTATUS kStatusSuccess = 0L;
    DECLARE_DLL_FUNCTION(RtlGetVersion, NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW), "ntdll.dll");
    if (!RtlGetVersion) {
        return;
    }

    RTL_OSVERSIONINFOW ovi { sizeof(ovi) };
    if (RtlGetVersion(&ovi) != kStatusSuccess) {
        return;
    }

    version_number.major_version = ovi.dwMajorVersion;
    version_number.minor_version = ovi.dwMinorVersion;
}

}   // namespace

namespace kbase {

// static
OSInfo* OSInfo::GetInstance()
{
    return Singleton<OSInfo, LeakySingletonTraits<OSInfo>>::instance();
}

OSInfo::OSInfo()
    : architecture_(SystemArchitecture::UNKNOWN_ARCHITECTURE),
      wow64_status_(GetWOW64StatusForProcess(GetCurrentProcess())),
      is_server_(IsWindowsServer()),
      processor_model_name_(GetProcessorModelName())
{
    GetSystemVersion(version_number_);

    SYSTEM_INFO system_info { 0 };
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

bool OSInfo::IsVersionOrGreater(SystemVersion version) const noexcept
{
    const VersionNumber& test_version_number = version_name_to_number.at(version);
    return !(version_number_ < test_version_number);
}

// static
WOW64Status OSInfo::GetWOW64StatusForProcess(HANDLE process) noexcept
{
    DECLARE_DLL_FUNCTION(IsWow64Process, BOOL(WINAPI*)(HANDLE, BOOL*), "kernel32.dll");
    if (!IsWow64Process) {
        return WOW64Status::WOW64_DISABLED;
    }

    BOOL is_wow64 = FALSE;
    if (!IsWow64Process(process, &is_wow64)) {
        return WOW64Status::WOW64_UNKNOWN;
    }

    return is_wow64 ? WOW64Status::WOW64_ENABLED : WOW64Status::WOW64_DISABLED;
}

bool operator<(const OSInfo::VersionNumber& lhs, const OSInfo::VersionNumber& rhs) noexcept
{
    return (lhs.major_version < rhs.major_version) ||
           (lhs.major_version == rhs.major_version && lhs.minor_version < rhs.minor_version);
}

}   // namespace kbase