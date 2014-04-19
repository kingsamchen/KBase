/*
 @ Kingsley Chen
*/

#include "kbase/logging.h"

#include <ctime>
#include <iomanip>

#include <Windows.h>

namespace kbase {

namespace {

const char* log_severity_names[] = {"INFO", "WARNING", "ERROR"};

LogItemOptions log_item_options = LogItemOptions::ENABLE_TIMESTAMP;

LoggingDestination logging_dest = LoggingDestination::LOG_TO_FILE;

}   // namespace

LogMessage::LogMessage(const char* file, int line, LogSeverity severity)
    : file_(file), line_(line), severity_(severity)
{
    Init(file, line);
}

LogMessage::~LogMessage()
{
    __debugbreak();
}

void LogMessage::Init(const char* file, int line)
{
    stream_ << "[";
    
    if (log_item_options & LogItemOptions::ENABLE_PROCESS_ID) {
        stream_ << GetCurrentProcessId() << ":";
    } 
    
    if (log_item_options & LogItemOptions::ENABLE_THREAD_ID) {
        stream_ << GetCurrentThreadId() << ":";
    } 
    
    if (log_item_options & LogItemOptions::ENABLE_TIMESTAMP) {
        time_t time_now = time(nullptr);
        struct tm local_time_now;
        localtime_s(&local_time_now, &time_now);

        stream_ << std::setfill('0')
                << std::setw(2) << 1 + local_time_now.tm_mon
                << std::setw(2) << local_time_now.tm_mday
                << '/'
                << std::setw(2) << local_time_now.tm_hour
                << std::setw(2) << local_time_now.tm_min
                << std::setw(2) << local_time_now.tm_sec
                << ':';
    }

    stream_ << log_severity_names[severity_];

    // Due to the performance consideration, I decide not to use std::string and
    // neither StringPiece, since I also want to keep this file more independent
    // as possible
    auto extract_file_name = [](const char* file_path) -> const char* {
        const char* p = file_path;
        const char* last_pos = file_path;
        for (; *p != '\0'; ++p) {
            if (*p == '/' || *p == '\\') {
                last_pos = p;
            }
        }

        return last_pos + 1;
    };
    const char* file_name = extract_file_name(file);

    stream_ << ':' << file_name << '(' << line << ")]";
    message_start_ = stream_.tellp();
}

}   // namespace kbase