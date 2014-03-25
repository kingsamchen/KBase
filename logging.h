/*
 @ Kingsley Chen
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef KBASE_LOGGING_H_
#define KBASE_LOGGING_H_

#include <string>

namespace kbase {

typedef int LogSeverity;

class LogMessage {
public:
    LogMessage(const char* file, int line, LogSeverity severity, int ctr);

    LogMessage(const char* file, int line);

    LogMessage(const char* file, int line, LogSeverity severity);

    LogMessage(const char* file, int line, std::string* result);

    LogMessage(const char* file, int line, LogMessage severity, std::string* result);

    ~LogMessage();

private:
    //TODO: delete copy-ctor and assignment-op

private:
    const char* file_;
    int line_;
    LogSeverity severity_;
};

}   // namespace kbase

#endif  // KBASE_LOGGING_H_