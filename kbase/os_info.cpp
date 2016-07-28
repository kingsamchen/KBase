/*
 @ 0xCCCCCCCC
*/

#include "kbase/os_info.h"

#include <Windows.h>
#include <VersionHelpers.h>

#include "kbase/basic_types.h"
#include "kbase/registry.h"

namespace {

using kbase::SystemVersion;
using kbase::WOW64Status;

using VersionNumber = kbase::OSInfo::VersionNumber;

const VersionNumber kVersionTable[] {
    { 6, 0 },   // SystemVersion::WIN_VISTA
    { 6, 1 },   // SystemVersion::WIN_7
    { 6, 2 },   // SystemVersion::WIN_8
    { 6, 3 },   // SystemVersion::WIN_8_1
    { 10, 0 },  // SystemVersion::WIN_10
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

bool OSInfo::IsVersionOrGreater(SystemVersion version) const noexcept
{
    const auto& test_version_number = kVersionTable[enum_cast(version)];
    return !(version_number_ < test_version_number);
}

std::string OSInfo::SystemVersionName() const
{
    std::string version_literal = "Unknown";
    if (kVersionTable[enum_cast(SystemVersion::WIN_VISTA)] == version_number()) {
        version_literal = "Windows Vista";
    } else if (kVersionTable[enum_cast(SystemVersion::WIN_7)] == version_number()) {
        version_literal = "Windows 7";
    } else if (kVersionTable[enum_cast(SystemVersion::WIN_8)] == version_number()) {
        version_literal = "Windows 8";
    } else if (kVersionTable[enum_cast(SystemVersion::WIN_8_1)] == version_number()) {
        version_literal = "Windows 8.1";
    } else if (kVersionTable[enum_cast(SystemVersion::WIN_10)] == version_number()) {
        version_literal = "Windows 10";
    }

    return version_literal;
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

bool OSInfo::RunningOn64BitSystem() const noexcept
{
#if defined(_WIN64)
    UNUSED_VAR(wow64_status());
    return true;
#elif defined(_WIN32)
    auto wow = wow64_status();
    switch (wow) {
        case OSInfo::WOW64_ENABLED:
            return true;
            break;
        case OSInfo::WOW64_DISABLED:
            return false;
            break;
        default:
            return false;
            break;
    }
#endif
}

// static
uint64_t OSInfo::UpTime() noexcept
{
    unsigned long long unbiased_interrupt_time = 0;
    QueryUnbiasedInterruptTime(&unbiased_interrupt_time);
    return unbiased_interrupt_time / 10;
}

}   // namespace kbase