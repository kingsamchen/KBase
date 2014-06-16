/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_VERSION_UTIL_H_
#define KBASE_VERSION_UTIL_H_

#include <string>

typedef void* HANDLE;

namespace kbase {

enum Version {
    WIN_XP,
    WIN_XP_SP3,
    WIN_SERVER_2003,
    WIN_VISTA,      // Also includes Server 2008.
    WIN_7,              
    WIN_8,          // Also includes Server 2012.
    WIN_8_1,        // Also includes Server 2012 R2.
};

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

    OSInfo(const OSInfo&) = delete;
    OSInfo& operator=(const OSInfo&) = delete;

    static OSInfo* GetInstance();

    // Returns WOW64_ENABLED, if the process is running under WOW64.
    // Returns WOW64_DISABLED, if the process is 64-bit application, or the process
    // is running on 32-bit system.
    // Returns WOW64_UNKNOWN, if an error occurs.
    // The handle to a process must have PROCESS_QUERY_INFORMATION access right.
    static WOW64Status GetWOW64StatusForProcess(HANDLE process);

    // TODO: process_model_name functions

    // This function can be made static, but in order to be consistent with is_server
    // in syntax, I choose to leave it as a member function.
    bool IsVersionOrGreater(Version version) const;

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

    unsigned long processors() const
    {
        return processors_;
    }

    unsigned long allocation_granularity() const
    {
        return allocation_granularity_;
    }

private:
    OSInfo();
    ~OSInfo();

private:
    SystemArchitecture architecture_;
    WOW64Status wow64_status_;
    bool is_server_;
    unsigned long processors_;
    unsigned long allocation_granularity_;
    std::string processor_model_name;
};

}   // namespace kbase

#endif  // KBASE_VERSION_UTIL_H_