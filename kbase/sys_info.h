/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SYS_INFO_H_
#define KBASE_SYS_INFO_H_

#include <cstdint>

namespace kbase {

class SysInfo {
public:
    SysInfo() = delete;
    ~SysInfo() = delete;

    // Returns the number of logical cores on the machine.
    static unsigned long NumberOfProcessors();

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
};

}   // namespace kbase

#endif  // KBASE_SYS_INFO_H_