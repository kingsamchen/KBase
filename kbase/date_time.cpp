/*
 @ Kingsley Chen
*/

#include "kbase\date_time.h"

#include <cassert>
#include <stdexcept>

#include "kbase\error_exception_util.h"

namespace kbase {

DateTime::DateTime(time_t time)
    : time_(time)
{}

DateTime::DateTime(int year, int month, int day)
    : DateTime(year, month, day, 0, 0, 0)
{}

DateTime::DateTime(int year, int month, int day, int hour, int min, int sec)
{
    assert(year >= 1900);
    assert(month >= 1 && month <= 12);
    assert(day >= 1 && day <= 31);
    assert(hour >= 0 && hour <= 23);
    assert(min >= 0 && min <= 59);
    assert(sec >= 0 && sec <= 59);

    struct tm std_tm;
    std_tm.tm_year = year - 1900;
    std_tm.tm_mon = month - 1;
    std_tm.tm_mday = day;
    std_tm.tm_hour = hour;
    std_tm.tm_min = min;
    std_tm.tm_sec = sec;
    std_tm.tm_isdst = -1;

    time_ = _mktime64(&std_tm);
    if (time_ == -1) {
        throw std::invalid_argument("failed to convert to DateTime");
    }
}

DateTime::DateTime(const SYSTEMTIME& systime)
{
    DateTime tmp(
        static_cast<int>(systime.wYear), static_cast<int>(systime.wMonth),
        static_cast<int>(systime.wDay), static_cast<int>(systime.wHour),
        static_cast<int>(systime.wMinute), static_cast<int>(systime.wSecond));
    
    *this = tmp;
}

DateTime::DateTime(const FILETIME& filetime)
{
    FILETIME local_file_time;
    BOOL ret = FileTimeToLocalFileTime(&filetime, &local_file_time);
    ThrowLastErrorIf(!ret, "failed to convert FileTime to LocalFileTime");

    SYSTEMTIME systime;
    ret = FileTimeToSystemTime(&local_file_time, &systime);
    ThrowLastErrorIf(!ret, "failed to convert FileTime to SystemTime");

    DateTime tmp(systime);

    *this = tmp;
}

}   // namespace kbase