# Chrono Utils

Chrono utilities provide several handy functions to assist in using `std::chrono` with platform-specific time structures.

## On Windows

We have functions for conversion between a chrono `time_point` and `SYSTEMTIME` and `FILETIME`:

```c++
TimePoint TimePointFromWindowsFileTime(const FILETIME& filetime);

FILETIME TimePointToWindowsFileTime(TimePoint time_point);

TimePoint TimePointFromWindowsSystemTime(const SYSTEMTIME& sys_time);

SYSTEMTIME TimePointToWindowsSystemTime(TimePoint time_point);
```

## On POSIX

We have functions for conversion between a chrono `time_point` and `timespec`:

```c++
TimePoint TimePointFromTimespec(const timespec& timespec);

timespec TimePointToTimespec(TimePoint time_point);
```

## Explode to Date-Time Members

You can create a `TimeExplode` instance out of a chrono `time_point`.

`TimeExplode` contains date time members similar to `struct tm`, but offering more intuitive values.

```c++
struct TimeExplode {
    int year;           // 4-digit year like 2019
    int month;          // 1-based month (1 ~ 12 represent January ~ December)
    int day_of_month;   // 1-based day of month (1 ~ 31)
    int day_of_week;    // 0-based day of week (0 ~ 6 represent Sunday ~ Saturday)
    int hour;           // Hours in 24-hour clock since midnight (0 ~ 23)
    int minute;         // Minutes after the hour (0 ~ 59)
    int second;         // Seconds after the minute (0 ~ 60 and leap seconds considered)
    int64_t remainder;  // Sub-second resolution value
};
```

Local time and UTC time are both available; and you can choose your sub-second resolution in accordance with your need:

```c++
template<typename Resolution = std::chrono::seconds, typename Clock, typename Duration>
TimeExplode TimePointToLocalTimeExplode(std::chrono::time_point<Clock, Duration> time_point);

template<typename Resolution = std::chrono::seconds, typename Clock, typename Duration>
TimeExplode TimePointToUTCTimeExplode(std::chrono::time_point<Clock, Duration> time_point);
```