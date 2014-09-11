/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_DATE_TIME_H_
#define KBASE_DATE_TIME_H_
    
#include <WinBase.h>

#include <ctime>

namespace kbase {
namespace internal {

typedef __time64_t time_type;

}   // namespace internal

// This class is intended to be designed as of value type.
class DateTime {
public:
    explicit DateTime(time_t time);

    DateTime(int year, int month, int day);

    DateTime(int year, int month, int day, int hour, int min, int sec);

    explicit DateTime(const SYSTEMTIME& systime);

    explicit DateTime(const FILETIME& filetime);

    static DateTime Now();

private:
    internal::time_type time_;
};

}   // namespace kbase

#endif  // KBASE_DATE_TIME_H_