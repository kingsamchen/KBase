/*
 @ 0xCCCCCCCC
*/

#include "catch2/catch.hpp"

#include <iomanip>
#include <iostream>

#include "kbase/chrono_util.h"
#include "kbase/secure_c_runtime.h"
#include "kbase/string_util.h"

namespace {

#if defined(OS_WIN)

bool EqualSystemTime(const SYSTEMTIME& lhs, const SYSTEMTIME& rhs)
{
    return lhs.wYear == rhs.wYear &&
           lhs.wMonth == rhs.wMonth &&
           lhs.wDay == rhs.wDay &&
           lhs.wDayOfWeek == rhs.wDayOfWeek &&
           lhs.wHour == rhs.wHour &&
           lhs.wMinute == rhs.wMinute &&
           lhs.wSecond == rhs.wSecond &&
           lhs.wMilliseconds == rhs.wMilliseconds;
}

bool EqualFileTime(const FILETIME& lhs, const FILETIME& rhs)
{
    return lhs.dwLowDateTime == rhs.dwLowDateTime &&
           lhs.dwHighDateTime == rhs.dwHighDateTime;
}

#endif

void PrintTimePoint(kbase::TimePoint tp)
{
    auto time = std::chrono::system_clock::to_time_t(tp);
    tm local_time {0};
    kbase::SecureLocalTime(&time, &local_time);
    std::cout << std::put_time(&local_time, "%c") << std::endl;
}

}   // namespace

namespace kbase {

#if defined(OS_WIN)

TEST_CASE("Convertions on windows", "[ChronoUtil]")
{
    SECTION("conversion between systemtime")
    {
        SYSTEMTIME sys_time {0};
        GetLocalTime(&sys_time);
        auto current_tp = TimePointFromWindowsSystemTime(sys_time);

        PrintTimePoint(current_tp);

        auto converted_sys_time = TimePointToWindowsSystemTime(current_tp);

        REQUIRE(EqualSystemTime(sys_time, converted_sys_time));
    }

    SECTION("conversion between filetime")
    {
        SYSTEMTIME sys_time = TimePointToWindowsSystemTime(std::chrono::system_clock::now());
        TzSpecificLocalTimeToSystemTime(nullptr, &sys_time, &sys_time);

        FILETIME filetime {0};
        REQUIRE(SystemTimeToFileTime(&sys_time, &filetime));

        auto ftp = TimePointFromWindowsFileTime(filetime);
        PrintTimePoint(ftp);

        auto cvt_filetime = TimePointToWindowsFileTime(ftp);

        REQUIRE(EqualFileTime(filetime, cvt_filetime));
    }
}

#endif

}   // namespace kbase
