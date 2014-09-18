/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"

#include "kbase\date_time.h"

using namespace kbase;

class DateTimeTest : public ::testing::Test {
};

namespace {

struct TimeExplode {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    int ms;
    int weekday;
};

// Revealed from a demo.
const time_t criterion_time_value = 1410548809;
const TimeExplode criterion_local {2014, 9, 13, 3, 6, 49, 88, 6};    // +8
const TimeExplode criterion_utc {2014, 9, 12, 19, 6, 49, 88, 5};     // UTC
const FILETIME criterion_ft_utc{653561120, 30384405};

bool EqualToTimeExplode(const struct tm& lhs, const TimeExplode& rhs)
{
    return (lhs.tm_year + 1900 == rhs.year) &&
           (lhs.tm_mon + 1 == rhs.month) &&
           (lhs.tm_mday == rhs.day) &&
           (lhs.tm_hour == rhs.hour) &&
           (lhs.tm_min == rhs.min) &&
           (lhs.tm_sec == rhs.sec) &&
           (lhs.tm_wday == rhs.weekday);
}

bool EqualToTimeExplode(const SYSTEMTIME& lhs, const TimeExplode& rhs)
{
    return (lhs.wYear == rhs.year) &&
           (lhs.wMonth == rhs.month) &&
           (lhs.wDay == rhs.day) &&
           (lhs.wHour == rhs.hour) &&
           (lhs.wMinute == rhs.min) &&
           (lhs.wSecond == rhs.sec) &&
           (lhs.wMilliseconds == rhs.ms) &&
           (lhs.wDayOfWeek == rhs.weekday);
}

bool EqualFileTime(const FILETIME& lhs, const FILETIME& rhs)
{
    return lhs.dwHighDateTime == rhs.dwHighDateTime &&
           lhs.dwLowDateTime == rhs.dwLowDateTime;
}

}   // namespace

// ctors

TEST_F(DateTimeTest, CtorForTimeMember)
{
    DateTime date_time(2014, 9, 13, 3, 6, 49, 0);
    DateTime date_time_with_ms(2014, 9, 13, 3, 6, 49, 999);
    EXPECT_TRUE(date_time.AsTimeT() == criterion_time_value);
    EXPECT_TRUE(date_time_with_ms.AsTimeT() == criterion_time_value);
}

TEST_F(DateTimeTest, CtorForSystemTime)
{
    SYSTEMTIME time {2014, 9, 6, 13, 3, 6, 49, 123};
    DateTime date_time(time);

    EXPECT_EQ(date_time.AsTimeT(), criterion_time_value);
}

TEST_F(DateTimeTest, CtorForFileTime)
{
    SYSTEMTIME time{2014, 9, 6, 13, 3, 6, 49, 321};
    FILETIME file_time;
    SystemTimeToFileTime(&time, &file_time);
    DateTime date_time(file_time, false);

    EXPECT_EQ(date_time.AsTimeT(), criterion_time_value);
}

// conversions

TEST_F(DateTimeTest, ToLocalTime)
{
    DateTime date_time(2014, 9, 13, 3, 6, 49, 0);
    auto local_tm = date_time.ToLocalTm();

    EXPECT_TRUE(EqualToTimeExplode(local_tm, criterion_local));
    EXPECT_FALSE(EqualToTimeExplode(local_tm, criterion_utc));
}

TEST_F(DateTimeTest, ToUTCTime)
{
    DateTime date_time(2014, 9, 13, 3, 6, 49, 0);
    auto utc_tm = date_time.ToUTCTm();

    EXPECT_TRUE(EqualToTimeExplode(utc_tm, criterion_utc));
    EXPECT_FALSE(EqualToTimeExplode(utc_tm, criterion_local));
}

TEST_F(DateTimeTest, ToSystemTime)
{
    DateTime date_time(2014, 9, 13, 3, 6, 49, 88);
    SYSTEMTIME sys_local_time = date_time.ToSystemTime();

    EXPECT_TRUE(EqualToTimeExplode(sys_local_time, criterion_local));
}

TEST_F(DateTimeTest, ToFileTime)
{
    DateTime date_time(2014, 7, 17, 0, 44, 3, 698);
    FILETIME ft = date_time.ToFileTime();
    DateTime tmp(ft, true);
    EXPECT_TRUE(EqualFileTime(ft, criterion_ft_utc));
}

// comparisons

TEST_F(DateTimeTest, ComparisonOperator)
{
    DateTime long_time_before(1989, 6, 4, 1, 2, 3, 4);
    DateTime now = DateTime::Now();

    // equal and not equal
    DateTime now_copy = now;
    EXPECT_TRUE(now == now_copy);
    EXPECT_FALSE(now == long_time_before);
    EXPECT_TRUE(now != long_time_before);
    EXPECT_FALSE(now != now_copy);
 
    // less and greater
    EXPECT_TRUE(long_time_before < now);
    EXPECT_TRUE(now > long_time_before);
    EXPECT_TRUE(long_time_before <= now);
    EXPECT_TRUE(now >= long_time_before);
    EXPECT_TRUE(now >= now_copy);
    EXPECT_TRUE(now <= now_copy);
    EXPECT_FALSE(long_time_before > now);   
}