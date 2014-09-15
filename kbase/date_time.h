/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_DATE_TIME_H_
#define KBASE_DATE_TIME_H_
    
#include <windows.h>

#include <ctime>
#include <cstdint>
#include <string>

namespace kbase {
namespace internal {

struct time_type {
    time_type()
    {}

    time_type(__time64_t t, int ms)
        : major(t), milliseconds(ms)
    {}

    int64_t compare(const time_type& other) const
    {
        int64_t this_value = major * 1000 + milliseconds;
        int64_t other_value = other.major * 1000 + other.milliseconds;
        return this_value - other_value;
    }

    __time64_t major;
    int milliseconds;   // [0, 999]
};

}   // namespace internal

// DateTime represents an absolute point in *local* time, internally represented as
// a combination of the number of seconds since 1970/1/1 00:00 UTC, which is
// consistent with time_t, and an integer that provides millisecond precision.
// This class is intended to be designed as of value type.
// Due to the constraints of struct tm, the |year| must be greater or equal
// to 1900; otherwise, an exception would be thrown wihtin the contructor.
// Besides, because time_t is in second-unit whereas FILETIIME is in 100ns-unit,
// none of them is interchangeable with DateTime. Use with care when you need
// *widening* conversions to/from these types.
class DateTime {
public:
    explicit DateTime(time_t time);

    DateTime(int year, int month, int day);

    DateTime(int year, int month, int day, int hour, int min, int sec, int ms);

    // Must be in local time.
    explicit DateTime(const SYSTEMTIME& systime);

    // FILETIME is by default UTC-based whereas time_t is local-based.
    // Therefore there are a few convertions which may throw Win32Exception when
    // they fail.
    explicit DateTime(const FILETIME& filetime, bool in_utc = true);

    static DateTime Now();

    std::string ToString(const char* fmt);

    std::wstring ToString(const wchar_t* fmt);

    std::string ToUTCString(const char* fmt);

    std::wstring ToUTCString(const wchar_t* fmt);

    // comparisons

    friend inline bool operator==(const DateTime& lhs, const DateTime& rhs);

    friend inline bool operator!=(const DateTime& lhs, const DateTime& rhs);

    friend inline bool operator<(const DateTime& lhs, const DateTime& rhs);

    friend inline bool operator>(const DateTime& lhs, const DateTime& rhs);

    friend inline bool operator<=(const DateTime& lhs, const DateTime& rhs);

    friend inline bool operator>=(const DateTime& lhs, const DateTime& rhs);

    time_t AsTimeT() const;

    struct tm ToLocalTm() const;

    struct tm ToUTCTm() const;

    SYSTEMTIME ToSystemTime() const;

    FILETIME ToFileTime() const;

    FILETIME ToLocalFileTime() const;
    
private:
    internal::time_type time_;
};

inline bool operator==(const DateTime& lhs, const DateTime& rhs)
{
    return lhs.time_.compare(rhs.time_) == 0;
}

inline bool operator!=(const DateTime& lhs, const DateTime& rhs)
{
    return !(lhs == rhs);
}

inline bool operator<(const DateTime& lhs, const DateTime& rhs)
{
    return lhs.time_.compare(rhs.time_) < 0;
}

inline bool operator>(const DateTime& lhs, const DateTime& rhs)
{
    return lhs.time_.compare(rhs.time_) > 0;
}

inline bool operator<=(const DateTime& lhs, const DateTime& rhs)
{
    return !(lhs > rhs);
}

inline bool operator>=(const DateTime& lhs, const DateTime& rhs)
{
    return !(lhs < rhs);
}

}   // namespace kbase

#endif  // KBASE_DATE_TIME_H_