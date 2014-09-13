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

namespace kbase {
namespace internal {

typedef __time64_t time_type;

}   // namespace internal

// DateTime represents an absolute point in *local* time, internally represented as
// the number of seconds since 1970/1/1 00:00 UTC, which is consistent with time_t.
// This class is intended to be designed as of value type.
// Due to the constraints of struct tm, the |year| must be greater or equal
// to 1900; otherwise, an exception would be thrown wihtin the contructor.
// Beside, lack of support for microsecond-precision results in discrepancy with
// SYSTEMTIME or FILETIME. Use with care when you have to convert to one of such
// type, and also requires ms-precision.
class DateTime {
public:
    explicit DateTime(time_t time);

    DateTime(int year, int month, int day);

    DateTime(int year, int month, int day, int hour, int min, int sec);

    // Must be in local time.
    explicit DateTime(const SYSTEMTIME& systime);

    // FILETIME is by default UTC-based whereas time_t is local-based.
    // Therefore there are a few convertions which may throw Win32Exception when
    // they fail.
    explicit DateTime(const FILETIME& filetime, bool in_utc = true);

    static DateTime Now();

    time_t AsTimeT() const;

    struct tm ToLocalTm() const;

    struct tm ToUTCTm() const;

    // In local time.
    // Be wary of ms-precision problem.
    SYSTEMTIME ToSystemTime() const;

    FILETIME ToFileTime() const;

    FILETIME ToLocalFileTime() const;

private:
    internal::time_type time_;
};

}   // namespace kbase

#endif  // KBASE_DATE_TIME_H_