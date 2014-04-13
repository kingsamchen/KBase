/*
 @ Kingsley Chen
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_LOGGING_H_
#define KBASE_LOGGING_H_

#include <sstream>
#include <string>

namespace kbase {

// these log severities are used to index into the array |log_severity_names|
typedef int LogSeverity;

const LogSeverity LOG_INFO = 0;
const LogSeverity LOG_WARNING = 1;
const LogSeverity LOG_ERROR = 2;

#define COMPACT_LOG_EX_INFO(ClassName) \
    kbase::ClassName(__FILE__, __LINE__, kbase::LOG_INFO)
#define COMPACT_LOG_EX_WARNING(ClassName) \
    kbase::ClassName(__FILE__, __LINE__, kbase::LOG_WARNING)
#define COMPACT_LOG_EX_ERROR(ClassName) \
    kbase::ClassName(__FILE__, __LINE__, kbase::LOG_ERROR)

#define COMPACT_LOG_INFO COMPACT_LOG_EX_INFO(LogMessage)
#define COMPACT_LOG_WARNING COMPACT_LOG_EX_WARNING(LogMessage)
#define COMPACT_LOG_ERROR COMPACT_LOG_EX_ERROR(LogMessage)

#define LAZY_STREAM(stream, condition) !(condition) ? (void)0 : (stream)
#define LOG_STREAM(severity) COMPACT_LOG_##severity.stream()

#define LOG(severity) LAZY_STREAM(LOG_STREAM(severity), true)
#define LOG_IF(severity, condition) LAZY_STREAM(LOG_STREAM(severity), condition)

class LogMessage {
public:
    LogMessage(const char* file, int line, LogSeverity severity, int ctr);

    LogMessage(const char* file, int line);

    LogMessage(const char* file, int line, LogSeverity severity);

    LogMessage(const char* file, int line, std::string* result);

    LogMessage(const char* file, int line, LogMessage severity, std::string* result);

    ~LogMessage();

    std::ostream& stream()
    {
        return stream_;
    }

private:
    LogMessage(const LogMessage&) = delete;

    LogMessage& operator=(const LogMessage&) = delete;

private:
    const char* file_;
    int line_;
    LogSeverity severity_;
    std::ostringstream stream_;
};

}   // namespace kbase

#endif  // KBASE_LOGGING_H_