/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SYS_INFO_H_
#define KBASE_SYS_INFO_H_

#include <cstdint>
#include <string>

#include "kbase/basic_macros.h"

namespace kbase {

class Path;

class SysInfo {
public:
    SysInfo() = delete;

    ~SysInfo() = delete;

    DISALLOW_COPY(SysInfo);

    DISALLOW_MOVE(SysInfo);

    // Returns the number of logical cores on the machine.
    static unsigned long NumberOfProcessors();

    // Returns VM allocation granularity.
    static unsigned long AllocationGranularity();

    static std::string SystemArchitecture();

    static std::string ProcessorModelName();

    static std::string SystemVersion();

    // Returns the uptime of the system.
    // This duration is in millisecond-unit, and does not include time the system
    // spends in sleep or hibernation.
    static uint64_t Uptime();

    // Returns true, if the host system is server edition.
    // Returns false, otherwise.
    static bool ServerSystem();

    // Returns true, if the host system is 64-bit system.
    // Return false, if not or an error occured.
    static bool RunningOn64BitSystem();

    // The following AmountOf* functions would throw exception when they fail.

    // Returns the amount of actual physical memory, in bytes.
    static uint64_t AmountOfTotalPhysicalMemory();

    // Returns the amount of physical memory currently available, in bytes.
    static uint64_t AmountOfAvailablePhysicalMemory();

    // Returns the size of the user mode portion of the virtual address space of the
    // calling process, in bytes. This value may vary in different type of process,
    // processors, or configurations of the system.
    static uint64_t AmountOfTotalVirtualMemory();

    // Returns the amount of unreserved and uncommitted memory currently in user mode
    // portion of the virtual address space of the calling process, in bytes.
    static uint64_t AmountOfAvailableVirtualMemory();

    // Returns the total number of bytes on a disk that are available to the user,
    // i.e. quota related.
    static uint64_t AmountOfTotalDiskSpace(const Path& path);

    // Returns the total number of free bytes on a disk that are available to the
    // user, i.e. quota related.
    static uint64_t AmountOfFreeDiskSpace(const Path& path);
};

}   // namespace kbase

#endif  // KBASE_SYS_INFO_H_