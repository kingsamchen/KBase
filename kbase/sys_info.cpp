/*
 @ Kingsley Chen
*/

#include "sys_info.h"

#include <Windows.h>

#include "kbase\error_exception_util.h"
#include "kbase\version_util.h"

namespace kbase {

namespace {

uint64_t AmountOfMemory(DWORDLONG MEMORYSTATUSEX::* memory_field)
{
    MEMORYSTATUSEX memory_status = { sizeof(memory_status) };
    BOOL ret = GlobalMemoryStatusEx(&memory_status);
    ThrowLastErrorIf(!ret, "Failed to get global memory status!");

    return memory_status.*memory_field;
}

}   // namespace

// static
unsigned long SysInfo::NumberOfProcessors()
{
    return OSInfo::GetInstance()->processors();
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

}   // namespace kbase
