/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_VERSION_UTIL_H_
#define KBASE_VERSION_UTIL_H_

typedef void* HANDLE;

namespace kbase {

enum Version {
    VERSION_PRE_XP = 0,     // Not supported.
    VERSION_XP,
    VERSION_SERVER_2003,    // Also includes Windows XP Pro X64 and Server 2003 R2.
    VERSION_VISTA,          // Also includes Windows Server 2008.
    VERSION_7,              // Also includes Windows Server 2008 R2.
    VERSION_8,              // Also includes Windows Server 2012.
    VERSION_8_1,
    VERSION_UNKNOWN
};

enum VersionType {
    SUITE_HOME,
    SUITE_PROFESSIONAL,
    SUITE_SERVER,
    SUITE_UNKNOWN
};

// It is a singleton.
class OSInfo {
public:
    struct VersionNumber {
        int major;
        int minor;
        int build;
    };

    struct ServicePack {
        int major;
        int minor;
    };

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

    static WOW64Status GetWOW64StatusForProcess(HANDLE process);

    // TODO: a lot of properties.

private:
    OSInfo();

    ~OSInfo();

private:
    Version version_;
    VersionNumber version_number_;
    VersionType version_type;
    ServicePack service_pack;
    SystemArchitecture architecture_;
    WOW64Status wow64_status_;
};

}   // namespace kbase

#endif  // KBASE_VERSION_UTIL_H_