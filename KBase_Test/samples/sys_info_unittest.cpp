/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include <iostream>

#include "kbase\files\file_path.h"
#include "kbase\error_exception_util.h"
#include "kbase\sys_info.h"

#include "gtest\gtest.h"

class SysInfoTest : public ::testing::Test {

};

TEST_F(SysInfoTest, ProcessorInfo)
{
    // I have 4 logical processors on my machine.
    EXPECT_EQ(4, kbase::SysInfo::NumberOfProcessors());

    // Virtual memory allocation granularity for X86/X64 is 64KB i.e. 65536 bytes.
    EXPECT_EQ(65536, kbase::SysInfo::AllocationGranularity());

    // Get CPU model name.
    EXPECT_NE(0, kbase::SysInfo::ProcessorModelName().length());

    // Get system architecture.
    EXPECT_STREQ("X86_64", kbase::SysInfo::SystemArchitecture().c_str());
}

TEST_F(SysInfoTest, SystemInfo)
{
    // Returns a string that indicates the current system version.
    EXPECT_STREQ("Windows 7", kbase::SysInfo::SystemVersion().c_str());

    // Is the host system a server edition?
    EXPECT_FALSE(kbase::SysInfo::ServerSystem());

    // Is the host system 64-bit?
    EXPECT_TRUE(kbase::SysInfo::RunningOn64BitSystem());
}

TEST_F(SysInfoTest, MemoryInfo)
{
    EXPECT_LT(0, kbase::SysInfo::AmountOfTotalPhysicalMemory() >> 20);
    EXPECT_LT(0, kbase::SysInfo::AmountOfAvailableVirtualMemory() >> 20);
    EXPECT_LT(0, kbase::SysInfo::AmountOfTotalPhysicalMemory() >> 20);
    EXPECT_LT(0, kbase::SysInfo::AmountOfTotalVirtualMemory() >> 20);
}

TEST_F(SysInfoTest, DiskspaceInfo)
{
    EXPECT_LT(0, kbase::SysInfo::AmountOfFreeDiskSpace(kbase::FilePath(L"C:")) >> 20);
    EXPECT_LT(0, kbase::SysInfo::AmountOfTotalDiskSpace(kbase::FilePath(L"C:")) >> 20);

    // F: does not exist, and hence the following invocations would throw an exception.
    EXPECT_THROW(kbase::SysInfo::AmountOfFreeDiskSpace(kbase::FilePath(L"F:")), kbase::Win32Exception);
    EXPECT_THROW(kbase::SysInfo::AmountOfTotalDiskSpace(kbase::FilePath(L"F:")), kbase::Win32Exception);
}