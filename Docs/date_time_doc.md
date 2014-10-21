DateTime and DateTimeSpan
===

[TOC]

## DateTime

`DateTime` represents an absolute point in **local** time, internally represented as the number of milliseconds since 1970/1/1 00:00 UTC, which is consistent with time_t on second-precision.

This class is intended to be designed as of value type.

Due to the constraints of `struct tm`, the date-year must be greater or equal to 1900; otherwise, an exception would be thrown wihtin the contructor.

Besides, because `time_t` is in second-unit whereas `FILETIIME` is in 100ns-unit, none of them is interchangeable with `DateTime`. Use with care when you need **widening** conversions to/from these types.

### ctor/dtor/operator=

---

#### DateTime(time_t time) [*explicit*]

Constructs a `DateTime` object from a given `time_t`.

#### DateTime(const DateTime&) = default

Default copy-constructor.

#### DateTime(int year, int month, int day)

Constructs a `DateTime` object from a given year, month and day.

#### DateTime(int year, int month, int day, int hour, int min, int sec, int ms)

Construts a `DateTime` object from a given specific date-time information.

#### DateTime(const SYSTEMTIME& systime) [*explicit*]

Constructs a `DateTime` object from a given `SYSTEMTIME` structure.

The `SYSTEMTIME` structure is regarded as of local-time.

#### DateTime(const FILETIME& filetime, bool in_utc = true) [*explicit*]

Constructs a `DateTime` object from a given `FILETIME` structure.

`FILETIME` is by default UTC-based whereas `time_t` is local-based, that's why the funtion offers a bool-indicator.

#### DateTime& operator=(const DateTime&) = default

Default copy-assignment.

#### DateTime Now() [*static*]

Constructs a `DateTime` object that indicates the time point when funtion was called.

### Converstion to Other Time-types

---

#### int64_t raw_time_since_epoch() [*const*]

Returns the raw time since the epoch.

#### time_t AsTimeT() [*const*]

Returns a corresponding `time_t`.

#### struct tm ToLocalTm() [*const*]

Returns a corresponding local-time in `struct tm`.

#### struct tm ToUTCTm() [*const*]

Returns a corresponding UTC-time in `struct tm`.

#### SYSTEMTIME ToSystemTime() [*const*]

Returns a corresponding local-time in `SYSTEMTIME`.

#### FILETIME ToFileTime() [*const*]

Returns a corresponding UTC-time in `FILETIME`.

**NOTE:** conversion `FILETIME`->`DateTime`->`FILETIME` definitely loses precision, therefore avoid conversions in this case.

#### FILETIME ToLocalFileTime() [*const*]

Returns a corresponding local-time in `FILETIME`.

**NOTE:** conversion `FILETIME`->`DateTime`->`FILETIME` definitely loses precision, therefore avoid conversions in this case.

### Formatted Stringify

---

Returns a formatted string for `DateTime` object.

For details about format specifications, see [here](http://msdn.microsoft.com/en-us/library/fe06s4ak.aspx).

```c++
kbase::DateTime now = kbase::DateTime::Now();
std::cout << now.ToString("%#c");	// Tuesday, October 21, 2014 16:07:39
```

#### std::string DateTime::ToString(const char* fmt)
#### std::wstring DateTime::ToString(const wchar_t* fmt)
#### std::string DateTime::ToUTCString(const char* fmt)
#### std::wstring DateTime::ToUTCString(const wchar_t* fmt)

### Comparison and Arithmetics

---

#### DateTime& operator+=(const DateTimeSpan& span)
#### DateTime& operator-=(const DateTimeSpan& span)
#### bool operator==(const DateTime& lhs, const DateTime& rhs)
#### bool operator!=(const DateTime& lhs, const DateTime& rhs)
#### bool operator<(const DateTime& lhs, const DateTime& rhs);
#### bool operator>(const DateTime& lhs, const DateTime& rhs);
#### bool operator<=(const DateTime& lhs, const DateTime& rhs)
#### bool operator>=(const DateTime& lhs, const DateTime& rhs)

---

## DateTimeSpan

`DateTimeSpan` represents a time-interval, in milliseconds.

### ctor/dtor/operator=

---

#### DateTimeSpan()

Constructs a default `DateTimeSpan` object, that is, time-interval is 0.

#### DateTimeSpan(const DateTimeSpan&) = default

Default copy-constructor.

#### DateTimeSpan(int64_t time_span) [*explicit*]

Constructs an object from the given `time_span`.

#### DateTimeSpan& operator=(const DateTimeSpan&) = default

Default copy-assignment.

#### DateTimeSpan FromDays(int64_t days) [*static*]

Constructs from a day-granularity value.

#### DateTimeSpan FromHours(int64_t hours) [*static*]

Constructs from a hour-granularity value.

#### DateTimeSpan FromMinutes(int64_t mins) [*static*]

Constructs from a minute-granularity value.

#### DateTimeSpan FromSeconds(int64_t secs) [*static*]

Constructs from a second-granularity value.

### Conversion to integer

---

#### int64_t time_span() [*const*]
#### int64_t AsDays() [*const*]
#### int64_t AsHours() [*const*]
#### int64_t AsMinutes() [*const*]
#### int64_t AsSeconds() [*const*]

### Comparisons and Arithmetics

---

#### DateTimeSpan& operator++()
#### DateTimeSpan& operator--()
#### DateTimeSpan& operator+=(const DateTimeSpan& span)
#### DateTimeSpan& operator-=(const DateTimeSpan& span)
#### bool operator==(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
#### bool operator!=(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
#### bool operator<(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
#### bool operator>(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
#### bool operator<=(const DateTimeSpan& lhs, const DateTimeSpan& rhs)
#### bool operator>=(const DateTimeSpan& lhs, const DateTimeSpan& rhs)