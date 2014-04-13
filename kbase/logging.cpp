/*
 @ Kingsley Chen
*/

#include "kbase/logging.h"

namespace kbase {

namespace {

const char* log_severity_names[] = {"INFO", "WARNING", "ERROR"};

}   // namespace

LogMessage::LogMessage(const char* file, int line, LogSeverity severity)
    : file_(file), line_(line), severity_(severity)
{}

LogMessage::~LogMessage()
{}

}   // namespace kbase