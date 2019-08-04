/*
 @ 0xCCCCCCCC
*/

#if defined _MSC_VER
#pragma once
#endif

#ifndef KBASE_CHRONO_UTIL_H_
#define KBASE_CHRONO_UTIL_H_

#include <chrono>
#include <ctime>

#include "kbase/basic_macros.h"
#include "kbase/secure_c_runtime.h"

#if defined(OS_WIN)
#include <Windows.h>
#elif defined(OS_POSIX)
#include <time.h>
#endif

namespace kbase {

struct TimeExplode {
    int year;           // 4-digit year like 2019
    int month;          // 1-based month (1 ~ 12 represent January ~ December)
    int day_of_month;   // 1-based day of month (1 ~ 31)
    int day_of_week;    // 0-based day of week (0 ~ 6 represent Sunday ~ Saturday)
    int hour;           // Hours in 24-hour clock since midnight (0 ~ 23)
    int minute;         // Minutes after the hour (0 ~ 59)
    int second;         // Seconds after the minute (0 ~ 60 and leap seconds considered)
    int64_t remainder;  // Sub-second resolution value
};

namespace internal {

template<typename Resolution = std::chrono::seconds, typename Clock, typename Duration, typename F>
TimeExplode TimePointToDateTime(std::chrono::time_point<Clock, Duration> time_point, F cvt)
{
    namespace chrono = std::chrono;

    auto in_resolution =  chrono::duration_cast<Resolution>(time_point.time_since_epoch());
    auto remainder = in_resolution - chrono::duration_cast<chrono::seconds>(in_resolution);

    time_t raw_time = Clock::to_time_t(time_point);
    tm tm;
    cvt(&raw_time, &tm);

    return {
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_wday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec,
        remainder.count()
    };
}

}   // namespace internal

using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;

#if defined(OS_WIN)

TimePoint TimePointFromWindowsFileTime(const FILETIME& filetime);

FILETIME TimePointToWindowsFileTime(TimePoint time_point);

TimePoint TimePointFromWindowsSystemTime(const SYSTEMTIME& sys_time);

SYSTEMTIME TimePointToWindowsSystemTime(TimePoint time_point);

#elif defined(OS_POSIX)

TimePoint TimePointFromTimespec(const timespec& timespec);

timespec TimePointToTimespec(TimePoint time_point);

#endif

template<typename Resolution = std::chrono::seconds, typename Clock, typename Duration>
TimeExplode TimePointToLocalTimeExplode(std::chrono::time_point<Clock, Duration> time_point)
{
    return internal::TimePointToDateTime<Resolution>(time_point, kbase::SecureLocalTime);
}

template<typename Resolution = std::chrono::seconds, typename Clock, typename Duration>
TimeExplode TimePointToUTCTimeExplode(std::chrono::time_point<Clock, Duration> time_point)
{
    return internal::TimePointToDateTime<Resolution>(time_point, kbase::SecureUTCTime);
}

}   // namespace kbase

#endif  // KBASE_CHRONO_UTIL_H_
