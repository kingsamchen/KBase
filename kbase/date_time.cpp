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

DateTime::DateTime(const FILETIME& filetime, bool in_utc /* = true */)
{
    BOOL ret;
    FILETIME local_file_time;
    if (in_utc) {
        ret = FileTimeToLocalFileTime(&filetime, &local_file_time);
        ThrowLastErrorIf(!ret, "failed to convert FileTime to LocalFileTime");
    } else {
        local_file_time = filetime;
    }

    SYSTEMTIME systime;
    ret = FileTimeToSystemTime(&local_file_time, &systime);
    ThrowLastErrorIf(!ret, "failed to convert FileTime to SystemTime");

    DateTime tmp(systime);

    *this = tmp;
}

// static
DateTime DateTime::Now()
{
    return DateTime(_time64(nullptr));
}

time_t DateTime::AsTimeT() const
{
    return static_cast<time_t>(time_);
}

struct tm DateTime::ToLocalTm() const
{
    struct tm local_time;
    if (_localtime64_s(&local_time, &time_)) {
        throw std::invalid_argument("failed to convert to local time");
    }

    return local_time;
}

struct tm DateTime::ToUTCTm() const
{
    struct tm utc_time;
    if (_gmtime64_s(&utc_time, &time_)) {
        throw std::invalid_argument("failed to convert to utc time");
    }

    return utc_time;
}

SYSTEMTIME DateTime::ToSystemTime() const
{
    struct tm local_time = ToLocalTm();
    SYSTEMTIME sys_local_time;
    sys_local_time.wYear = static_cast<WORD>(local_time.tm_year + 1900);
    sys_local_time.wMonth = static_cast<WORD>(local_time.tm_mon + 1);
    sys_local_time.wDay = static_cast<WORD>(local_time.tm_mday);
    sys_local_time.wDayOfWeek = static_cast<WORD>(local_time.tm_wday);
    sys_local_time.wHour = static_cast<WORD>(local_time.tm_hour);
    sys_local_time.wMinute = static_cast<WORD>(local_time.tm_min);
    sys_local_time.wSecond = static_cast<WORD>(local_time.tm_sec);
    // not support for ms-precision.
    sys_local_time.wMilliseconds = 0;

    return sys_local_time;
}

FILETIME DateTime::ToFileTime() const
{
    FILETIME local_file_time = ToLocalFileTime();
    FILETIME utc_file_time;
    BOOL ret = LocalFileTimeToFileTime(&local_file_time, &utc_file_time);
    ThrowLastErrorIf(!ret, "failed to convert local file time to file time");

    return utc_file_time;
}

FILETIME DateTime::ToLocalFileTime() const
{
    FILETIME local_file_time;
    SYSTEMTIME sys_local_time = ToSystemTime();
    BOOL ret = SystemTimeToFileTime(&sys_local_time, &local_file_time);
    ThrowLastErrorIf(!ret, "failed to convert system time to file time");

    return local_file_time;
}

}   // namespace kbase