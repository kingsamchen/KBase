/*
 @ 0xCCCCCCCC
*/

#include "kbase/date_time.h"

#include <cassert>
#include <stdexcept>

#include "kbase/error_exception_util.h"

namespace kbase {

const int internal::time_type::kScaleRatio = 1000;

namespace {

inline int64_t ExpandTimeT(__time64_t t)
{
    return t * internal::time_type::kScaleRatio;
}

inline __time64_t ShrinkToTimeT(int64_t t)
{
    return t / internal::time_type::kScaleRatio;
}

template<typename strT, typename FmtFunc>
strT DateTimeToStringHelper(const struct tm& t,
                            const typename strT::value_type* fmt,
                            FmtFunc fn)
{
    const size_t kBufferSize = 256U;
    typename strT::value_type buffer[kBufferSize];

    if (!fn(buffer, kBufferSize, fmt, &t)) {
        return strT();
    }

    return strT(buffer);
}

}   // namespace

DateTime::DateTime(time_t time)
    : time_(ExpandTimeT(time))
{}

DateTime::DateTime(int year, int month, int day)
    : DateTime(year, month, day, 0, 0, 0, 0)
{}

DateTime::DateTime(int year, int month, int day, int hour, int min, int sec, int ms)
{
    assert(year >= 1970);
    assert(month >= 1 && month <= 12);
    assert(day >= 1 && day <= 31);
    assert(hour >= 0 && hour <= 23);
    assert(min >= 0 && min <= 59);
    assert(sec >= 0 && sec <= 59);
    assert(ms >= 0 && ms <= 999);

    struct tm std_tm;
    std_tm.tm_year = year - 1900;
    std_tm.tm_mon = month - 1;
    std_tm.tm_mday = day;
    std_tm.tm_hour = hour;
    std_tm.tm_min = min;
    std_tm.tm_sec = sec;
    std_tm.tm_isdst = -1;

    time_t since_epoch = _mktime64(&std_tm);
    if (since_epoch == -1) {
        throw std::invalid_argument("failed to convert to DateTime");
    }

    time_.time_value = ExpandTimeT(since_epoch) + ms;
}

DateTime::DateTime(const SYSTEMTIME& systime)
{
    DateTime tmp(
        static_cast<int>(systime.wYear), static_cast<int>(systime.wMonth),
        static_cast<int>(systime.wDay), static_cast<int>(systime.wHour),
        static_cast<int>(systime.wMinute), static_cast<int>(systime.wSecond),
        static_cast<int>(systime.wMilliseconds));

    *this = tmp;
}

DateTime::DateTime(const FILETIME& filetime, bool in_utc /* = true */)
{
    BOOL ret;
    FILETIME local_file_time;
    if (in_utc) {
        ret = FileTimeToLocalFileTime(&filetime, &local_file_time);
        ENSURE(RAISE, ret != 0)(LastError()).Require("Failed to convert FileTime to LocalFileTime");
    } else {
        local_file_time = filetime;
    }

    SYSTEMTIME systime;
    ret = FileTimeToSystemTime(&local_file_time, &systime);
    ENSURE(RAISE, ret != 0)(LastError()).Require("Failed to convert FileTime to SystemTime");

    DateTime tmp(systime);

    *this = tmp;
}

// static
DateTime DateTime::Now()
{
    SYSTEMTIME now;
    GetLocalTime(&now);

    return DateTime(now);
}

std::string DateTime::ToString(const char* fmt)
{
    auto t = ToLocalTm();
    return DateTimeToStringHelper<std::string>(t, fmt, strftime);
}

std::wstring DateTime::ToString(const wchar_t* fmt)
{
    auto t = ToLocalTm();
    return DateTimeToStringHelper<std::wstring>(t, fmt, wcsftime);
}

std::string DateTime::ToUTCString(const char* fmt)
{
    auto t = ToUTCTm();
    return DateTimeToStringHelper<std::string>(t, fmt, strftime);
}

std::wstring DateTime::ToUTCString(const wchar_t* fmt)
{
    auto t = ToUTCTm();
    return DateTimeToStringHelper<std::wstring>(t, fmt, wcsftime);
}

time_t DateTime::AsTimeT() const
{
    return static_cast<time_t>(ShrinkToTimeT(time_.time_value));
}

struct tm DateTime::ToLocalTm() const
{
    __time64_t t = ShrinkToTimeT(time_.time_value);
    struct tm local_time;
    if (_localtime64_s(&local_time, &t)) {
        throw std::invalid_argument("failed to convert to local time");
    }

    return local_time;
}

struct tm DateTime::ToUTCTm() const
{
    __time64_t t = ShrinkToTimeT(time_.time_value);
    struct tm utc_time;
    if (_gmtime64_s(&utc_time, &t)) {
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
    sys_local_time.wMilliseconds = static_cast<WORD>(
        time_.time_value % internal::time_type::kScaleRatio);

    return sys_local_time;
}

FILETIME DateTime::ToFileTime() const
{
    FILETIME local_file_time = ToLocalFileTime();
    FILETIME utc_file_time;
    BOOL ret = LocalFileTimeToFileTime(&local_file_time, &utc_file_time);
    ENSURE(RAISE, ret != 0)(LastError()).Require("Failed to convert local file time to file time");

    return utc_file_time;
}

FILETIME DateTime::ToLocalFileTime() const
{
    FILETIME local_file_time;
    SYSTEMTIME sys_local_time = ToSystemTime();
    BOOL ret = SystemTimeToFileTime(&sys_local_time, &local_file_time);
    ENSURE(RAISE, ret != 0)(LastError()).Require("Failed to convert system time to file time");

    return local_file_time;
}

// --* DateTimeSpan *--

DateTimeSpan::DateTimeSpan()
    : time_span_(0)
{}

DateTimeSpan::DateTimeSpan(int64_t time_span)
    : time_span_(time_span)
{}

// static
DateTimeSpan DateTimeSpan::FromDays(int64_t days)
{
    return DateTimeSpan(days * 86400 * 1000);
}

// static
DateTimeSpan DateTimeSpan::FromHours(int64_t hours)
{
    return DateTimeSpan(hours * 3600 * 1000);
}

// static
DateTimeSpan DateTimeSpan::FromMinutes(int64_t mins)
{
    return DateTimeSpan(mins * 60 * 1000);
}

// static
DateTimeSpan DateTimeSpan::FromSeconds(int64_t secs)
{
    return DateTimeSpan(secs * 1000);
}

}   // namespace kbase