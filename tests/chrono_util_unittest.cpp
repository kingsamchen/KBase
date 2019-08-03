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

}   // namespace

namespace kbase {

#if defined(OS_WIN)

TEST_CASE("Convertions on windows", "[ChronoUtil]")
{
    SECTION("conversion between systemtime") {
        SYSTEMTIME sys_time {0};
        GetLocalTime(&sys_time);
        auto current_tp = TimePointFromWindowsSystemTime(sys_time);

        PrintTimePoint(current_tp);

        auto converted_sys_time = TimePointToWindowsSystemTime(current_tp);

        REQUIRE(EqualSystemTime(sys_time, converted_sys_time));
    }

    SECTION("conversion between filetime") {
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

TEST_CASE("Convert TimePoint to local time explode", "[ChronoUtil]")
{
    auto now = std::chrono::system_clock::now();

    struct tm tm;
    auto t = std::chrono::system_clock::to_time_t(now);
    kbase::SecureLocalTime(&t, &tm);

    auto explode = TimePointToLocalTimeExplode(now);

    CHECK(explode.year == tm.tm_year + 1900);
    CHECK(explode.month == tm.tm_mon + 1);
    CHECK(explode.day_of_month == tm.tm_mday);
    CHECK(explode.day_of_week == tm.tm_wday);
    CHECK(explode.hour == tm.tm_hour);
    CHECK(explode.minute == tm.tm_min);
    CHECK(explode.second == tm.tm_sec);

    // For second-resolution, remainder should be 0.
    CHECK(explode.remainder == 0);

    SECTION("milisecond-resolution explode should differ only in remainder") {
        auto ep_ms = TimePointToLocalTimeExplode<std::chrono::milliseconds>(now);
        CHECK(ep_ms.remainder > 0);
        CHECK(ep_ms.year == explode.year);
        CHECK(explode.month == ep_ms.month);
        CHECK(explode.day_of_month == ep_ms.day_of_month);
        CHECK(explode.day_of_week == ep_ms.day_of_week);
        CHECK(explode.hour == ep_ms.hour);
        CHECK(explode.minute == ep_ms.minute);
        CHECK(explode.second == ep_ms.second);
    }
}

TEST_CASE("Convert TimePoint to utc time explode", "[ChronoUtil]")
{
    auto now = std::chrono::system_clock::now();

    struct tm tm;
    auto t = std::chrono::system_clock::to_time_t(now);
    kbase::SecureUTCTime(&t, &tm);

    auto explode = TimePointToUTCTimeExplode(now);

    CHECK(explode.year == tm.tm_year + 1900);
    CHECK(explode.month == tm.tm_mon + 1);
    CHECK(explode.day_of_month == tm.tm_mday);
    CHECK(explode.day_of_week == tm.tm_wday);
    CHECK(explode.hour == tm.tm_hour);
    CHECK(explode.minute == tm.tm_min);
    CHECK(explode.second == tm.tm_sec);

    // For second-resolution, remainder should be 0.
    CHECK(explode.remainder == 0);

    SECTION("milisecond-resolution explode should differ only in remainder") {
        auto ep_ms = TimePointToUTCTimeExplode<std::chrono::milliseconds>(now);
        CHECK(ep_ms.remainder > 0);
        CHECK(ep_ms.year == explode.year);
        CHECK(explode.month == ep_ms.month);
        CHECK(explode.day_of_month == ep_ms.day_of_month);
        CHECK(explode.day_of_week == ep_ms.day_of_week);
        CHECK(explode.hour == ep_ms.hour);
        CHECK(explode.minute == ep_ms.minute);
        CHECK(explode.second == ep_ms.second);
    }
}

}   // namespace kbase
