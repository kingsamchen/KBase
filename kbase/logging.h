/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_LOGGING_H_
#define KBASE_LOGGING_H_

#include <sstream>
#include <string>

#include "kbase\basic_macros.h"

namespace kbase {

// These log severities will be used as index into the array `log_severity_names`.
enum class LogSeverity : int {
    LOG_INFO = 0,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
};

namespace internal {

#if !defined(NDEBUG)
#define ENABLE_DLOG 1
#else
#define ENABLE_DLOG 0
#endif

enum { DLOG_ON = ENABLE_DLOG };

#undef ENABLE_DLOG

}   // namespace internal

#define COMPACT_LOG_EX_INFO(ClassName) \
    kbase::ClassName(__FILE__, __LINE__, kbase::LogSeverity::LOG_INFO)
#define COMPACT_LOG_EX_WARNING(ClassName) \
    kbase::ClassName(__FILE__, __LINE__, kbase::LogSeverity::LOG_WARNING)
#define COMPACT_LOG_EX_ERROR(ClassName) \
    kbase::ClassName(__FILE__, __LINE__, kbase::LogSeverity::LOG_ERROR)
#define COMPACT_LOG_EX_FATAL(ClassName) \
    kbase::ClassName(__FILE__, __LINE__, kbase::LogSeverity::LOG_FATAL)

// Surprisingly, a macro `ERROR` is defined as 0 in file <wingdi.h>, which is
// included by <windows.h>, so we add a special macro to handle this peculiar
// chaos, in case the file was included.
#define COMPACT_LOG_INFO    COMPACT_LOG_EX_INFO(LogMessage)
#define COMPACT_LOG_WARNING COMPACT_LOG_EX_WARNING(LogMessage)
#define COMPACT_LOG_ERROR   COMPACT_LOG_EX_ERROR(LogMessage)
#define COMPACT_LOG_0       COMPACT_LOG_EX_ERROR(LogMessage)
#define COMPACT_LOG_FATAL   COMPACT_LOG_EX_FATAL(LogMessage)

#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void)0 : kbase::LogMessageVoidfy() & (stream)
#define LOG_STREAM(severity) COMPACT_LOG_ ## severity.stream()

#define LOG(severity) LAZY_STREAM(LOG_STREAM(severity), true)
#define LOG_IF(severity, condition) LAZY_STREAM(LOG_STREAM(severity), condition)

#define DLOG(severity) LAZY_STREAM(LOG_STREAM(severity), ::kbase::internal::DLOG_ON)
#define DLOG_IF(severity, condition) \
    LAZY_STREAM(LOG_STREAM(severity), ::kbase::internal::DLOG_ON && condition)

enum LogItemOptions {
    ENABLE_NONE = 0,
    ENABLE_PROCESS_ID = 1 << 0,
    ENABLE_THREAD_ID = 1 << 1,
    ENABLE_TIMESTAMP = 1 << 2,
    ENABLE_ALL = ENABLE_PROCESS_ID | ENABLE_THREAD_ID | ENABLE_TIMESTAMP
};

enum LoggingDestination {
    LOG_NONE = 0,
    LOG_TO_FILE = 1 << 0,
    LOG_TO_SYSTEM_DEBUG_LOG = 1 << 1,
    LOG_TO_ALL = LOG_TO_FILE | LOG_TO_SYSTEM_DEBUG_LOG
};

enum OldFileDisposalOption {
    APPEND_TO_OLD_LOG_FILE,
    DELETE_OLD_LOG_FILE
};

enum LoggingLockOption {
    USE_GLOBAL_LOCK,
    USE_LOCAL_LOCK
};

struct LoggingSettings {
    // Initializes settings to default values.
    LoggingSettings();

    LogItemOptions log_item_options;
    LoggingDestination logging_destination;
    OldFileDisposalOption old_file_disposal_option;
    LoggingLockOption logging_lock_option;
};

// You should better initialize these settings before logging facility being used.
// If you don't call this function to initialize logging settings, default settings
// are applied.
// Note that, this function must not be called more than once.
void InitLoggingSettings(const LoggingSettings& settings);

class LogMessage {
public:
    LogMessage(const char* file, int line);

    LogMessage(const char* file, int line, LogSeverity severity);

    ~LogMessage();

    DISALLOW_COPY(LogMessage);

    DISALLOW_MOVE(LogMessage);

    std::ostream& stream()
    {
        return stream_;
    }

private:
    // Writes the common info header into the stream.
    // The info header is in the following format:
    //  [pid:tid:mmdd/hhmmss:severity:filename(line)]
    // in which, pid tid and logging time all are optional, and are controlled by
    // logging setting.
    void Init(const char* file, int line);

private:
    const char* file_;
    int line_;
    LogSeverity severity_;
    std::ostringstream stream_;
};

// This class is used to suppress the compiler warning like "std::ostream object cannot
// be copied" since conditional statement in macro |LAZY_STREAM| may return an
// std::ostream object.
class LogMessageVoidfy {
public:
    LogMessageVoidfy() {}
    void operator&(std::ostream&) {}
};

}   // namespace kbase

#endif  // KBASE_LOGGING_H_