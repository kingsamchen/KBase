/*
 @ 0xCCCCCCCC
*/

#include <thread>

#include "catch2/catch.hpp"

#include "kbase/os_info.h"

namespace kbase {

TEST_CASE("Query uptime since last boot", "[OSInfo]")
{
    auto uptime = OSInfo::UpTime();
    REQUIRE(uptime.count() > 0);
    INFO("uptime: " << uptime.count() << " seconds\n");
}

#if defined(OS_WIN)

TEST_CASE("Query WOW64 state", "[OSInfo]")
{
    auto wow64_status = OSInfo::GetWOW64StatusForProcess(GetCurrentProcess());
    bool host_64bit = OSInfo::RunningOn64BitSystem();
#if defined(_WIN64)
    bool app_64bit = true;
#else
    bool app_64bit = true;
#endif

    if (host_64bit && !app_64bit) {
        REQUIRE(WOW64Status::Enabled == wow64_status);
    } else {
        REQUIRE(WOW64Status::Disabled == wow64_status);
    }
}

#endif

TEST_CASE("Acquire system version information", "[OSInfo]")
{
#if defined(OS_WIN)
    // OSInfo::IsVersionOrGreater
    REQUIRE(OSInfo::GetInstance()->IsVersionOrGreater(SystemVersion::WinVista));
    REQUIRE(OSInfo::GetInstance()->IsVersionOrGreater(SystemVersion::Win7));
    REQUIRE(OSInfo::GetInstance()->IsVersionOrGreater(SystemVersion::Win8));
    REQUIRE(OSInfo::GetInstance()->IsVersionOrGreater(SystemVersion::Win8_1));
    REQUIRE(OSInfo::GetInstance()->IsVersionOrGreater(SystemVersion::Win10));
#endif

    std::string ver_name = OSInfo::GetInstance()->SystemName();
    INFO(ver_name);
    REQUIRE(ver_name != "Unknown");

    auto ver_num = OSInfo::GetInstance()->version_number();
    REQUIRE((ver_num.major_ver > 0 && ver_num.minor_ver >= 0));
    INFO("Version number: " << ver_num.major_ver << "." << ver_num.minor_ver << "\n");
}

TEST_CASE("Misc features", "[OSInfo]")
{
    REQUIRE(SystemArchitecture::X86_64 == OSInfo::GetInstance()->architecture());

    REQUIRE(OSInfo::GetInstance()->number_of_cores() >= 1UL);
    INFO("num-of-cores: " << OSInfo::GetInstance()->number_of_cores() << "\n");

    REQUIRE(OSInfo::GetInstance()->vm_granularity() > 0UL);
    INFO("allocation-granularity: " << OSInfo::GetInstance()->vm_granularity() << "\n");
}

}   // namespace kbase
