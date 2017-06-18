/*
 @ 0xCCCCCCCC
*/

#include <thread>

#include "gtest/gtest.h"

#include "kbase/os_info.h"

namespace kbase {

TEST(OSInfoTest, UpTime)
{
    auto uptime = OSInfo::UpTime();
    EXPECT_TRUE(uptime.count() > 0);
    std::cout << "uptime: " << uptime.count() << " seconds\n";
}

#if defined(OS_WIN)
TEST(OSInfoTest, WOW64)
{
    auto wow64_status = OSInfo::GetWOW64StatusForProcess(GetCurrentProcess());
    bool host_64bit = OSInfo::RunningOn64BitSystem();
#if defined(_WIN64)
    bool app_64bit = true;
#else
    bool app_64bit = true;
#endif

    if (host_64bit && !app_64bit) {
        EXPECT_EQ(WOW64Status::Enabled, wow64_status);
    } else {
        EXPECT_EQ(WOW64Status::Disabled, wow64_status);
    }
}
#endif

TEST(OSInfoTest, Version)
{

#if defined(OS_WIN)
    // OSInfo::IsVersionOrGreater
    EXPECT_TRUE(OSInfo::GetInstance()->IsVersionOrGreater(SystemVersion::WinVista));
    EXPECT_TRUE(OSInfo::GetInstance()->IsVersionOrGreater(SystemVersion::Win7));
    EXPECT_TRUE(OSInfo::GetInstance()->IsVersionOrGreater(SystemVersion::Win8));
    EXPECT_TRUE(OSInfo::GetInstance()->IsVersionOrGreater(SystemVersion::Win8_1));
    EXPECT_TRUE(OSInfo::GetInstance()->IsVersionOrGreater(SystemVersion::Win10));
#endif

    std::string ver_name = OSInfo::GetInstance()->SystemName();
    std::cout << ver_name << std::endl;
    EXPECT_TRUE(ver_name != "Unknown");

    auto ver_num = OSInfo::GetInstance()->version_number();
    EXPECT_TRUE(ver_num.major_ver > 0 && ver_num.minor_ver >= 0);
    std::cout << "Version number: " << ver_num.major_ver << "." << ver_num.minor_ver << "\n";
}

TEST(OSInfoTest, Misc)
{
    EXPECT_EQ(SystemArchitecture::X86_64, OSInfo::GetInstance()->architecture());

    EXPECT_GE(OSInfo::GetInstance()->number_of_cores(), 1UL);
    std::cout << "num-of-cores: " << OSInfo::GetInstance()->number_of_cores() << "\n";

    EXPECT_GT(OSInfo::GetInstance()->vm_granularity(), 0UL);
    std::cout << "allocation-granularity: " << OSInfo::GetInstance()->vm_granularity() << "\n";
}

}   // namespace kbase
