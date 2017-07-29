/*
 @ 0xCCCCCCCC
*/

#if defined _MSC_VER
#pragma once
#endif

#ifndef KBASE_CHRONO_UTIL_H_
#define KBASE_CHRONO_UTIL_H_

#include <chrono>

#include "kbase/basic_macros.h"

#if defined(OS_WIN)
#include <Windows.h>
#endif

namespace kbase {

using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;

#if defined(OS_WIN)

TimePoint TimePointFromWindowsFileTime(const FILETIME& filetime);

FILETIME TimePointToWindowsFileTime(TimePoint time_point);

TimePoint TimePointFromWindowsSystemTime(const SYSTEMTIME& sys_time);

SYSTEMTIME TimePointToWindowsSystemTime(TimePoint time_point);

#endif

}   // namespace kbase

#endif  // KBASE_CHRONO_UTIL_H_
