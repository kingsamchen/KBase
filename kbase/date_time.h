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

// This class is intended to be designed as of value type.
// Due to the constraints of struct time_t, the |year| must be greater or equal
// to 1900; otherwise, an exception would be thrown wihtin the contructor.
class DateTime {
public:
    explicit DateTime(time_t time);

    DateTime(int year, int month, int day);

    DateTime(int year, int month, int day, int hour, int min, int sec);

    explicit DateTime(const SYSTEMTIME& systime);

    // FILETIME is by default UTC-based whereas time_t is local-based.
    // Therefore there are a few convertions which may throw Win32Exception when
    // they fail.
    explicit DateTime(const FILETIME& filetime);

    static DateTime Now();

private:
    internal::time_type time_;
};

}   // namespace kbase

#endif  // KBASE_DATE_TIME_H_