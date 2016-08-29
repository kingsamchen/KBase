/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include <thread>

#include "gtest/gtest.h"

#include "kbase/os_info.h"
#include "kbase/string_encoding_conversions.h"

using kbase::OSInfo;
using kbase::SystemVersion;
using kbase::WOW64Status;
using kbase::SystemArchitecture;

class OSInfoTest : public ::testing::Test {
public:
    static void SetUpTestCase()
    {
        instance_ = OSInfo::GetInstance();
    }

    static OSInfo* instance()
    {
        return instance_;
    }

    // It is used only in Singleton test.
    OSInfo* instance_ptr = nullptr;

private:
    static OSInfo* instance_;
};

TEST_F(OSInfoTest, Singleton)
{
    for (int i = 0; i < 100; ++i) {
        std::thread th([&]() {
            if (!instance_ptr) {
                instance_ptr = OSInfo::GetInstance();
            } else {
                ASSERT_EQ(instance_ptr, OSInfo::GetInstance());
            }
        });

        th.join();
    }
}

OSInfo* OSInfoTest::instance_;

TEST_F(OSInfoTest, SysVersion)
{
    // OSInfo::IsVersionOrGreater
    EXPECT_TRUE(instance()->IsVersionOrGreater(SystemVersion::WIN_VISTA));
    EXPECT_TRUE(instance()->IsVersionOrGreater(SystemVersion::WIN_7));
    EXPECT_TRUE(instance()->IsVersionOrGreater(SystemVersion::WIN_8));
    EXPECT_TRUE(instance()->IsVersionOrGreater(SystemVersion::WIN_8_1));
    EXPECT_TRUE(instance()->IsVersionOrGreater(SystemVersion::WIN_10));

    std::string ver_name = instance()->SystemVersionName();
    std::cout << ver_name << std::endl;
    EXPECT_TRUE(ver_name != "Unknown");
}

TEST_F(OSInfoTest, SysArchitecture)
{
    EXPECT_TRUE(instance()->RunningOn64BitSystem());

    auto processor_model = instance()->ProcessorModelName();
    std::cout << kbase::WideToASCII(processor_model) << std::endl;
    EXPECT_FALSE(processor_model.empty());

    EXPECT_FALSE(instance()->IsServerSystem());

    EXPECT_EQ(SystemArchitecture::X64_ARCHITECTURE, instance()->architecture());

    EXPECT_NE(0, instance()->NumberOfProcessors());

    EXPECT_EQ(65536, instance()->AllocationGranularity());
}

TEST_F(OSInfoTest, UpTime)
{
    auto time = OSInfo::UpTime();
    EXPECT_TRUE(time > 0);
    std::cout << time / 1000 / 1000 / 3600 << std::endl;
}