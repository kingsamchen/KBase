/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include <thread>

#include "kbase\version_util.h"

#include "gtest\gtest.h"

using kbase::OSInfo;

class OSInfoTest : public ::testing::Test {
public:
    static void SetUpTestCase()
    {
        instance_ = OSInfo::GetInstance();
    }

    OSInfo* instance()
    {
        return instance_;
    }

    // It is used only in Singleton test.
    OSInfo* instance_ptr = nullptr;

private:
    static OSInfo* instance_;
};

namespace {

bool CompareVersionNumber(const OSInfo::VersionNumber& lhs, const OSInfo::VersionNumber& rhs)
{
    return lhs.major_version == rhs.major_version &&
           lhs.minor_version == rhs.minor_version;
}

}   // namespace

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
    EXPECT_TRUE(instance()->IsVersionOrGreater(OSInfo::WIN_XP));
    EXPECT_TRUE(instance()->IsVersionOrGreater(OSInfo::WIN_VISTA));
    EXPECT_TRUE(instance()->IsVersionOrGreater(OSInfo::WIN_7));
    EXPECT_TRUE(instance()->IsVersionOrGreater(OSInfo::WIN_8));
    EXPECT_TRUE(instance()->IsVersionOrGreater(OSInfo::WIN_8_1));
}

TEST_F(OSInfoTest, SysArchitecture)
{
    EXPECT_EQ(OSInfo::WOW64_ENABLED, instance()->wow64_status());

    EXPECT_NE(0, instance()->processor_model_name().length());

    EXPECT_FALSE(instance()->is_server());

    EXPECT_EQ(OSInfo::X64_ARCHITECTURE, instance()->architecture());

    EXPECT_NE(0, instance()->processors());

    EXPECT_EQ(65536, instance()->allocation_granularity());
}