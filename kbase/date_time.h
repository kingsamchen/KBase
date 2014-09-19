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
        : time_value(0)
    {}

    time_type(int64_t val)
        : time_value(val)
    {}

    int64_t compare(const time_type& other) const
    {
        return time_value - other.time_value;
    }

    static const int kScaleRatio;

    // number of milliseconds since 1970/1/1 00:00 UTC.
    int64_t time_value;
};

}   // namespace internal

// DateTime represents an absolute point in *local* time, internally represented as
// the number of milliseconds since 1970/1/1 00:00 UTC, which is consistent with
// time_t on second-precision.
// This class is intended to be designed as of value type.
// Due to the constraints of struct tm, the |year| must be greater or equal
// to 1900; otherwise, an exception would be thrown wihtin the contructor.
// Besides, because time_t is in second-unit whereas FILETIIME is in 100ns-unit,
// none of them is interchangeable with DateTime. Use with care when you need
// *widening* conversions to/from these types.
class DateTime {
public:
    explicit DateTime(time_t time);

    DateTime(const DateTime&) = default;

    DateTime& operator=(const DateTime&) = default;

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

    // conversions

    inline int64_t raw_time_since_epoch() const;

    time_t AsTimeT() const;

    struct tm ToLocalTm() const;

    struct tm ToUTCTm() const;

    SYSTEMTIME ToSystemTime() const;

    FILETIME ToFileTime() const;

    FILETIME ToLocalFileTime() const;
    
private:
    internal::time_type time_;
};

inline int64_t DateTime::raw_time_since_epoch() const
{
    return time_.time_value;
}

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

// This class represents a time interval, in millisecond-unit.
class DateTimeSpan {
public:
    DateTimeSpan();

    DateTimeSpan(const DateTimeSpan&) = default;

    DateTimeSpan(int64_t time_span);

    DateTimeSpan& operator=(const DateTimeSpan&) = default;

    inline int64_t time_span() const;

    inline DateTimeSpan& operator++();

    inline DateTimeSpan& operator--();

    inline DateTimeSpan& operator+=(const DateTimeSpan& span);

    inline DateTimeSpan& operator-=(const DateTimeSpan& span);

    friend inline bool operator==(const DateTimeSpan& lhs, const DateTimeSpan& rhs);

    friend inline bool operator!=(const DateTimeSpan& lhs, const DateTimeSpan& rhs);

    friend inline bool operator<(const DateTimeSpan& lhs, const DateTimeSpan& rhs);

    friend inline bool operator>(const DateTimeSpan& lhs, const DateTimeSpan& rhs);

    friend inline bool operator<=(const DateTimeSpan& lhs, const DateTimeSpan& rhs);

    friend inline bool operator>=(const DateTimeSpan& lhs, const DateTimeSpan& rhs);

private:
    int64_t time_span_;
};

inline int64_t DateTimeSpan::time_span() const
{
    return time_span_;
}

// arithmetics

inline DateTimeSpan& DateTimeSpan::operator++()
{
    ++time_span_;

    return *this;
}

inline DateTimeSpan& DateTimeSpan::operator--()
{
    --time_span_;

    return *this;
}

inline DateTimeSpan& DateTimeSpan::operator+=(const DateTimeSpan& span)
{
    time_span_ += span.time_span_;

    return *this;
}

inline DateTimeSpan& DateTimeSpan::operator-=(const DateTimeSpan& span)
{
    time_span_ -= span.time_span_;

    return *this;
}

inline const DateTimeSpan operator+(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
{
    DateTimeSpan ret(lhs);
    ret += rhs;

    return ret;
}

inline const DateTimeSpan operator-(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
{
    DateTimeSpan ret(lhs);
    ret -= rhs;

    return ret;
}

// comparisons

inline bool operator==(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
{
    return lhs.time_span_ == rhs.time_span_;
}

inline bool operator!=(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
{
    return !(lhs == rhs);
}

inline bool operator<(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
{
    return lhs.time_span_ < rhs.time_span_;
}

inline bool operator>(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
{
    return lhs.time_span_ > rhs.time_span_;
}

inline bool operator<=(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
{
    return !(lhs > rhs);
}

inline bool operator>=(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
{
    return !(lhs < rhs);
}

}   // namespace kbase

#endif  // KBASE_DATE_TIME_H_