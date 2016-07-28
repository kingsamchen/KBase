/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_OS_INFO_H_
#define KBASE_OS_INFO_H_

#include <string>

#include "kbase/basic_macros.h"
#include "kbase/singleton.h"

using HANDLE = void*;

namespace kbase {

enum class SystemArchitecture {
    X86_ARCHITECTURE,
    X64_ARCHITECTURE,
    IA64_ARCHITECTURE,
    UNKNOWN_ARCHITECTURE
};

enum class WOW64Status {
    WOW64_DISABLED,
    WOW64_ENABLED,
    WOW64_UNKNOWN
};

// Doesn't support versions prior to Windows Vista.
enum class SystemVersion : unsigned int {
    WIN_VISTA,      // Also includes Server 2008.
    WIN_7,          // Also includes Server 2008 R2.
    WIN_8,          // Also includes Server 2012.
    WIN_8_1,        // Also includes Server 2012 R2.
    WIN_10,
};

// It is a singleton.
class OSInfo {
public:
    struct VersionNumber {
        VersionNumber() noexcept
            : major_version(0), minor_version(0)
        {}

        VersionNumber(unsigned long major, unsigned long minor) noexcept
            : major_version(major), minor_version(minor)
        {}

        unsigned long major_version;
        unsigned long minor_version;
    };

    ~OSInfo() = default;

    DISALLOW_COPY(OSInfo);

    DISALLOW_MOVE(OSInfo);

    static OSInfo* GetInstance();

    // Returns the uptime of the system.
    // This duration is in microseconds, and does not include time the system
    // spends in sleep or hibernation.
    static uint64_t UpTime() noexcept;

    // Returns WOW64_ENABLED, if the process is running under WOW64.
    // Returns WOW64_DISABLED, if the process is 64-bit application, or the process
    // is running on 32-bit system.
    // Returns WOW64_UNKNOWN, if an error occurs.
    // The handle to a process must have PROCESS_QUERY_INFORMATION access right.
    static WOW64Status GetWOW64StatusForProcess(HANDLE process) noexcept;

    // Returns true, if the host system is 64-bit system.
    // Return false, if not or an error occured.
    bool RunningOn64BitSystem() const noexcept;

    bool IsVersionOrGreater(SystemVersion version) const noexcept;

    std::string SystemVersionName() const;

    const std::wstring& ProcessorModelName() const noexcept
    {
        return processor_model_name_;
    }

    bool IsServerSystem() const noexcept
    {
        return is_server_;
    }

    unsigned long NumberOfProcessors() const noexcept
    {
        return processors_;
    }

    unsigned long AllocationGranularity() const noexcept
    {
        return allocation_granularity_;
    }

    SystemArchitecture architecture() const noexcept
    {
        return architecture_;
    }

    VersionNumber version_number() const noexcept
    {
        return version_number_;
    }

private:
    OSInfo();

    WOW64Status wow64_status() const noexcept
    {
        return wow64_status_;
    }

    friend DefaultSingletonTraits<OSInfo>;

private:
    SystemArchitecture architecture_;
    WOW64Status wow64_status_;
    bool is_server_;
    std::wstring processor_model_name_;
    VersionNumber version_number_;
    unsigned long processors_;
    unsigned long allocation_granularity_;
};

inline bool operator==(const OSInfo::VersionNumber& lhs, const OSInfo::VersionNumber& rhs) noexcept
{
    return lhs.major_version == rhs.major_version && lhs.minor_version == rhs.minor_version;
}

inline bool operator<(const OSInfo::VersionNumber& lhs, const OSInfo::VersionNumber& rhs) noexcept
{
    return (lhs.major_version < rhs.major_version) ||
           (lhs.major_version == rhs.major_version && lhs.minor_version < rhs.minor_version);
}

}   // namespace kbase

#endif  // KBASE_OS_INFO_H_