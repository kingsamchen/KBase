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

typedef void* HANDLE;
typedef unsigned short WORD;

namespace kbase {

// It is a singleton.
class OSInfo {
public:
    enum SystemArchitecture {
        X86_ARCHITECTURE,
        X64_ARCHITECTURE,
        IA64_ARCHITECTURE,
        UNKNOWN_ARCHITECTURE
    };

    enum WOW64Status {
        WOW64_DISABLED,
        WOW64_ENABLED,
        WOW64_UNKNOWN
    };

    enum Version {
        WIN_XP,
        WIN_XP_SP3,
        WIN_SERVER_2003,
        WIN_VISTA,      // Also includes Server 2008.
        WIN_7,
        WIN_8,          // Also includes Server 2012.
        WIN_8_1,        // Also includes Server 2012 R2.
    };

    struct VersionNumber {
        WORD major_version;
        WORD minor_version;
        WORD service_pack_major;
    };

    DISALLOW_COPY(OSInfo);

    DISALLOW_MOVE(OSInfo);

    static OSInfo* GetInstance();

    // Returns WOW64_ENABLED, if the process is running under WOW64.
    // Returns WOW64_DISABLED, if the process is 64-bit application, or the process
    // is running on 32-bit system.
    // Returns WOW64_UNKNOWN, if an error occurs.
    // The handle to a process must have PROCESS_QUERY_INFORMATION access right.
    static WOW64Status GetWOW64StatusForProcess(HANDLE process);

    // This function can be made static, but in order to be consistent with is_server
    // in syntax, I choose to leave it as a member function.
    bool IsVersionOrGreater(Version version) const;

    std::wstring processor_model_name() const
    {
        return processor_model_name_;
    }

    bool is_server() const
    {
        return is_server_;
    }

    SystemArchitecture architecture() const
    {
        return architecture_;
    }

    WOW64Status wow64_status() const
    {
        return wow64_status_;
    }

    VersionNumber version_number() const
    {
        return version_number_;
    }

    unsigned long processors() const
    {
        return processors_;
    }

    unsigned long allocation_granularity() const
    {
        return allocation_granularity_;
    }

    void DestroyInstance()
    {
        delete this;
    }

private:
    friend DefaultSingletonTraits<OSInfo>;
    OSInfo();
    ~OSInfo();

    std::wstring GetProcessorModelName() const;

private:
    SystemArchitecture architecture_;
    WOW64Status wow64_status_;
    VersionNumber version_number_;
    bool is_server_;
    unsigned long processors_;
    unsigned long allocation_granularity_;
    std::wstring processor_model_name_;
};

}   // namespace kbase

#endif  // KBASE_OS_INFO_H_