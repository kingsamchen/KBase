/*
 @ 0xCCCCCCCC
*/

#include "sys_info.h"

#include <Windows.h>

#include <tuple>

#include "kbase/error_exception_util.h"
#include "kbase/path.h"
#include "kbase/sys_string_encoding_conversions.h"
#include "kbase/os_info.h"

namespace kbase {

namespace {

uint64_t AmountOfMemory(DWORDLONG MEMORYSTATUSEX::* memory_field)
{
    MEMORYSTATUSEX memory_status = { sizeof(memory_status) };
    BOOL ret = GlobalMemoryStatusEx(&memory_status);
    ENSURE(RAISE, ret != 0)(LastError()).Require("Failed to get global memory status!");

    return memory_status.*memory_field;
}

typedef std::tuple<ULARGE_INTEGER*, ULARGE_INTEGER*, ULARGE_INTEGER*> SpaceType;

void GetDiskSpace(const Path& path, const SpaceType& type)
{
    BOOL ret = GetDiskFreeSpaceEx(path.value().c_str(),
                                  std::get<0>(type),
                                  std::get<1>(type),
                                  std::get<2>(type));
    ENSURE(RAISE, ret != 0)(LastError()).Require("Failed to get disk space information!");
}

}   // namespace

// static
unsigned long SysInfo::NumberOfProcessors()
{
    return OSInfo::GetInstance()->NumberOfProcessors();
}

// static
unsigned long SysInfo::AllocationGranularity()
{
    return OSInfo::GetInstance()->AllocationGranularity();
}

// static
std::string SysInfo::SystemArchitecture()
{
    std::string str;
    auto architecture = OSInfo::GetInstance()->architecture();
    switch (architecture) {
        case SystemArchitecture::X86_ARCHITECTURE:
            str = "X86";
            break;
        case SystemArchitecture::X64_ARCHITECTURE:
            str = "X86_64";
            break;
        case SystemArchitecture::IA64_ARCHITECTURE:
            str = "IA64";
            break;
        default:
            str = "";
            break;
    }

    return str;
}

// static
std::string SysInfo::ProcessorModelName()
{
    std::wstring cpu_model_name = OSInfo::GetInstance()->processor_model_name();
    return SysWideToNativeMB(cpu_model_name);
}

// static
std::string SysInfo::SystemVersion()
{
    std::string version;
    auto ver = OSInfo::GetInstance()->version_number();
    if (ver.major_version == 5 && ver.minor_version == 1) {
        version = "Windows XP";
    } else if (ver.major_version == 5 && ver.minor_version == 2) {
        version = "Windows Server 2003";
    } else if (ver.major_version == 6 && ver.minor_version == 0) {
        version = "Windows Vista";
    } else if (ver.major_version == 6 && ver.minor_version == 1) {
        version = "Windows 7";
    } else if (ver.major_version == 6 && ver.minor_version == 2) {
        version = "Windows 8";
    } else if (ver.major_version == 6 && ver.minor_version == 3) {
        version = "Windows 8.1";
    } else {
        version = "";
    }

    return version;
}

// static
uint64_t SysInfo::Uptime()
{
    unsigned long long raw_time = 0;
    BOOL ret = QueryUnbiasedInterruptTime(&raw_time);
    ENSURE(RAISE, ret != 0)(LastError()).Require("Failed to query unbiased interrupt time");

    return raw_time / (10 * 1000);
}

// static
bool SysInfo::ServerSystem()
{
    return OSInfo::GetInstance()->is_server();
}

// static
bool SysInfo::RunningOn64BitSystem()
{
#if defined(_WIN64)
    return true;
#elif defined(_WIN32)
    auto wow = OSInfo::GetInstance()->wow64_status();
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
uint64_t SysInfo::AmountOfTotalPhysicalMemory()
{
    return AmountOfMemory(&MEMORYSTATUSEX::ullTotalPhys);
}

// static
uint64_t SysInfo::AmountOfAvailablePhysicalMemory()
{
    return AmountOfMemory(&MEMORYSTATUSEX::ullAvailPhys);
}

// static
uint64_t SysInfo::AmountOfTotalVirtualMemory()
{
    return AmountOfMemory(&MEMORYSTATUSEX::ullTotalVirtual);
}

// static
uint64_t SysInfo::AmountOfAvailableVirtualMemory()
{
    return AmountOfMemory(&MEMORYSTATUSEX::ullAvailVirtual);
}

// static
uint64_t SysInfo::AmountOfTotalDiskSpace(const Path& path)
{
    ULARGE_INTEGER total;
    GetDiskSpace(path, std::make_tuple(nullptr, &total, nullptr));

    return total.QuadPart;
}

// static
uint64_t SysInfo::AmountOfFreeDiskSpace(const Path& path)
{
    ULARGE_INTEGER free;
    GetDiskSpace(path, std::make_tuple(&free, nullptr, nullptr));

    return free.QuadPart;
}

}   // namespace kbase
