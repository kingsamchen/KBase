/*
 @ 0xCCCCCCCC
*/

#include <vector>

#include "gtest/gtest.h"

#include "kbase/scoped_handle.h"

namespace kbase {

TEST(ScopedHandleTest, Normal)
{
    {
        ScopedHandle sys_h;
        EXPECT_FALSE(sys_h);
    }

    {
        ScopedHandle event_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
        EXPECT_TRUE(!!event_h);
        std::cout << "event_h underlying handle: " << event_h.get() << std::endl;
    }
}

TEST(ScopedHandleTest, TestAndNullize)
{
    ScopedHandle event_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
    if (!event_h) {
        ASSERT_TRUE(false);
    }
    auto h = event_h.release();
    EXPECT_FALSE(static_cast<bool>(event_h));
    EXPECT_NE(h, nullptr);
    event_h = nullptr;
    event_h.reset(h);
    EXPECT_NE(static_cast<bool>(event_h), false);
    event_h = nullptr;
    EXPECT_FALSE(static_cast<bool>(event_h));
}

TEST(ScopedHandleTest, MoveSemantics)
{
    {
        ScopedHandle event_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
        ASSERT_TRUE(static_cast<bool>(event_h));
        ScopedHandle another_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
        ASSERT_TRUE(static_cast<bool>(event_h));
        ASSERT_NE(event_h.get(), another_h.get());

        auto reserved_h = event_h.get();
        auto reserved_another_h = another_h.get();
        event_h = std::move(another_h);

        EXPECT_FALSE(static_cast<bool>(another_h));
        EXPECT_TRUE(static_cast<bool>(event_h));
        EXPECT_EQ(reserved_another_h, event_h.get());
        BOOL rv = CloseHandle(reserved_h);
        DWORD err = GetLastError();
        EXPECT_FALSE(rv);
        EXPECT_EQ(err, ERROR_INVALID_HANDLE);
    }

    {
        std::vector<ScopedHandle> events;
        for (int i = 0; i < 10; ++i) {
            events.emplace_back(CreateEventW(nullptr, TRUE, TRUE, nullptr));
        }
    }
}

}   // namespace kbase
