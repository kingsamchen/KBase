/*
 @ Kingsley Chen
*/

#include "kbase\date_time.h"

#include <cassert>
#include <stdexcept>

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
    if (systime.wYear < 1900) {
        throw std::invalid_argument("failed to convert to DateTime");
    }

    // TODO: make DateTime from systime
}

}   // namespace kbase