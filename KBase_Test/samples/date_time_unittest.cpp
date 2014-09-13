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
    int weekday;
};

// Revealed from a demo.
const time_t criterion_time_value = 1410548809;
const TimeExplode criterion_local = {2014, 9, 13, 3, 6, 49, 6};    // +8
const TimeExplode criterion_utc = {2014, 9, 12, 19, 6, 49, 5};     // UTC

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
           (lhs.wDayOfWeek == rhs.weekday);
}

}   // namespace

// ctors

TEST_F(DateTimeTest, CtorForTimeMember)
{
    DateTime date_time(2014, 9, 13, 3, 6, 49);
    EXPECT_TRUE(date_time.AsTimeT() == criterion_time_value);
}

TEST_F(DateTimeTest, CtorForSystemTime)
{
    SYSTEMTIME time {2014, 9, 6, 13, 3, 6, 49};
    DateTime date_time(time);

    EXPECT_EQ(date_time.AsTimeT(), criterion_time_value);
}

TEST_F(DateTimeTest, CtorForFileTime)
{
    SYSTEMTIME time{2014, 9, 6, 13, 3, 6, 49};
    FILETIME file_time;
    SystemTimeToFileTime(&time, &file_time);
    DateTime date_time(file_time, false);

    EXPECT_EQ(date_time.AsTimeT(), criterion_time_value);
}

// conversions

TEST_F(DateTimeTest, ToLocalTime)
{
    DateTime date_time(2014, 9, 13, 3, 6, 49);
    auto local_tm = date_time.ToLocalTm();

    EXPECT_TRUE(EqualToTimeExplode(local_tm, criterion_local));
    EXPECT_FALSE(EqualToTimeExplode(local_tm, criterion_utc));
}

TEST_F(DateTimeTest, ToUTCTime)
{
    DateTime date_time(2014, 9, 13, 3, 6, 49);
    auto utc_tm = date_time.ToUTCTm();

    EXPECT_TRUE(EqualToTimeExplode(utc_tm, criterion_utc));
    EXPECT_FALSE(EqualToTimeExplode(utc_tm, criterion_local));
}

TEST_F(DateTimeTest, ToSystemTime)
{
    DateTime date_time(criterion_time_value);
    SYSTEMTIME sys_local_time = date_time.ToSystemTime();

    EXPECT_TRUE(EqualToTimeExplode(sys_local_time, criterion_local));
}