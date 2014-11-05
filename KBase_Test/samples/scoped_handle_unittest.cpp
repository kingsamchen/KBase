
#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\memory\scoped_handle.h"

using kbase::ScopedSysHandle;
using kbase::ScopedStdioHandle;

TEST(ScopedHandleTest, Normal)
{
    {
        ScopedSysHandle sys_h;
        EXPECT_FALSE(sys_h);
        ScopedStdioHandle stdio_h;
        EXPECT_FALSE(stdio_h);
    }

    ScopedSysHandle::Handle h = nullptr;
    {
        ScopedSysHandle event_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
        EXPECT_TRUE(static_cast<bool>(event_h));
        std::cout << "event_h underlying handle: " << event_h.Get() << std::endl;
        h = event_h.Get();
    }
    EXPECT_TRUE(h != nullptr);
    BOOL rv = CloseHandle(h);
    DWORD err = GetLastError();
    EXPECT_FALSE(rv);
    EXPECT_EQ(err, ERROR_INVALID_HANDLE);    

    ScopedStdioHandle::Handle fh = nullptr;
    {
        FILE* fp = nullptr;
        fopen_s(&fp, "C:\\Windows\\win.ini", "r");
        ASSERT_TRUE(fp != nullptr);
        std::cout << "FILE*: " << fp << std::endl;
        ScopedStdioHandle fh(fp);
        EXPECT_TRUE(static_cast<bool>(fh));
        std::cout << "stdio underlying handle: " << fh.Get() << std::endl;
    }
}

TEST(ScopedHandleTest, TestAndNullize)
{
    ScopedSysHandle event_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
    if (!event_h) {
        ASSERT_TRUE(false);
    }
    auto h = event_h.Release();
    EXPECT_FALSE(static_cast<bool>(event_h));
    EXPECT_NE(h, nullptr);
    event_h = nullptr;
    event_h.Reset(h);
    EXPECT_NE(static_cast<bool>(event_h), false);
    event_h = nullptr;
    EXPECT_FALSE(static_cast<bool>(event_h));
}

TEST(ScopedHandleTest, MoveSemantics)
{
    {
        FILE* fp = nullptr;
        fopen_s(&fp, "C:\\test.t", "w");
        ScopedStdioHandle fh(fp);
        ASSERT_TRUE(static_cast<bool>(fh));
        fwrite("abcd", sizeof(char), 4, static_cast<FILE*>(fh));
        ScopedStdioHandle new_fh(std::move(fh));
        EXPECT_FALSE(static_cast<bool>(fh));
        EXPECT_TRUE(static_cast<bool>(new_fh));
        fwrite("1234", sizeof(char), 4, static_cast<FILE*>(new_fh));
        new_fh = nullptr;
        remove("C:\\test.t");
    }    

    {
        ScopedSysHandle event_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
        ASSERT_TRUE(static_cast<bool>(event_h));
        ScopedSysHandle another_h(CreateEventW(nullptr, TRUE, TRUE, nullptr));
        ASSERT_TRUE(static_cast<bool>(event_h));
        ASSERT_NE(event_h.Get(), another_h.Get());
        
        auto reserved_h = static_cast<HANDLE>(event_h);
        auto reserved_another_h = static_cast<HANDLE>(another_h);
        event_h = std::move(another_h);
        
        EXPECT_FALSE(static_cast<bool>(another_h));
        EXPECT_TRUE(static_cast<bool>(event_h));
        EXPECT_EQ(reserved_another_h, event_h.Get());
        BOOL rv = CloseHandle(reserved_h);
        DWORD err = GetLastError();
        EXPECT_FALSE(rv);
        EXPECT_EQ(err, ERROR_INVALID_HANDLE);
    }
}