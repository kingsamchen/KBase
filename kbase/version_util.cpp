/*
 @ Kingsley Chen
*/

#include "kbase\version_util.h"

#include <map>
#include <memory>

#include <Windows.h>
#include <VersionHelpers.h>

#include "kbase\registry.h"

namespace {

using Version = kbase::OSInfo::Version;
using VersionNumber = kbase::OSInfo::VersionNumber;
using WOW64Status = kbase::OSInfo::WOW64Status;

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

namespace kbase {

// static
OSInfo* OSInfo::GetInstance()
{
    return Singleton<OSInfo, LeakySingletonTraits<OSInfo>>::instance();
}

OSInfo::OSInfo()
    : architecture_(UNKNOWN_ARCHITECTURE),
      wow64_status_(GetWOW64StatusForProcess(GetCurrentProcess())),
      is_server_(IsWindowsServer()),
      processor_model_name_(GetProcessorModelName())
{
    // GetVersionEx was declared deprecated since Windows 8.1. You should use the
    // VersionNumber struct only when you need to query what exact system version you
    // are using. see
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms724451(v=vs.85).aspx
    // for more details.
#pragma warning(push)
#pragma warning(disable: 4996 28159)
    OSVERSIONINFOEX os_version_info = { sizeof(os_version_info) };
    GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&os_version_info));
    version_number_.major_version =
        static_cast<WORD>(os_version_info.dwMajorVersion);
    version_number_.minor_version =
        static_cast<WORD>(os_version_info.dwMinorVersion);
    version_number_.service_pack_major =
        static_cast<WORD>(os_version_info.wServicePackMajor);
#pragma warning(pop)

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

bool OSInfo::IsVersionOrGreater(Version version) const
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

std::wstring OSInfo::GetProcessorModelName() const
{
    const wchar_t kKeyName[] = L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";

    RegKey key(HKEY_LOCAL_MACHINE);
    key.OpenKey(kKeyName, KEY_READ);
    std::wstring processor_model_name;
    key.ReadValue(L"ProcessorNameString", &processor_model_name);

    return processor_model_name;
}

}   // namespace kbase