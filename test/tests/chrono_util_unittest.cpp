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

void PrintTimePoint(kbase::TimePoint tp)
{
    auto time = std::chrono::system_clock::to_time_t(tp);
    tm local_time {0};
    kbase::SecureLocalTime(&time, &local_time);
    std::cout << std::put_time(&local_time, "%c") << std::endl;
}

#endif

bool EqualTM(const struct tm& lhs, const struct tm& rhs)
{
    return lhs.tm_year == rhs.tm_year &&
           lhs.tm_mon == rhs.tm_mon &&
           lhs.tm_mday == rhs.tm_mday &&
           lhs.tm_yday == rhs.tm_yday &&
           lhs.tm_hour == rhs.tm_hour &&
           lhs.tm_min == rhs.tm_min &&
           lhs.tm_sec == rhs.tm_sec &&
           lhs.tm_wday == rhs.tm_wday &&
           lhs.tm_isdst == rhs.tm_isdst;
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

TEST_CASE("Convert TimePoint to tm and remainder as local time", "[ChronoUtil]")
{
    auto now = std::chrono::system_clock::now();
    auto result_sec = TimePointToLocalTime(now);
    auto result_ms = TimePointToLocalTime<std::chrono::milliseconds>(now);
    REQUIRE(EqualTM(result_sec.first, result_ms.first));
    REQUIRE(result_sec.second.count() == 0);
    REQUIRE(result_ms.second.count() > 0);
    std::cout << std::put_time(&result_ms.first, "%c") << ", " << result_ms.second.count() << "\n";
}

TEST_CASE("Convert TimePoint to tm and remainder as utc", "[ChronoUtil]")
{
    auto now = std::chrono::system_clock::now();
    auto result_sec = TimePointToUTCTime(now);
    auto result_us = TimePointToUTCTime<std::chrono::microseconds>(now);
    REQUIRE(EqualTM(result_sec.first, result_us.first));
    REQUIRE(result_sec.second.count() == 0);
    REQUIRE(result_us.second.count() > 0);
    std::cout << std::put_time(&result_us.first, "%c") << ", " << result_us.second.count() << "\n";
}

}   // namespace kbase
