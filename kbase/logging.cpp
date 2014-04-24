/*
 @ Kingsley Chen
*/

#include "kbase/logging.h"

#include <ctime>
#include <iomanip>

#include <Windows.h>

#include "strings/sys_string_encoding_conversions.h"

namespace kbase {

namespace {

const char* log_severity_names[] = {"INFO", "ERROR", "FATAL"};

LogItemOptions log_item_options = LogItemOptions::ENABLE_TIMESTAMP;

LoggingDestination logging_dest = LoggingDestination::LOG_TO_FILE;

OldFileOption old_file_option = OldFileOption::APPEND_TO_OLD_LOG_FILE;

typedef wchar_t PathChar;
typedef std::wstring PathString;

const PathChar kFileNameSuffix[] = L"debug_message.log";

// Due to the performance consideration, I decide not to use std::string and
// neither StringPiece, since I also want to keep this file more independent
// as possible
template<typename charT>
const charT* ExtractFileName(const charT* file_path)
{
    const charT* p = file_path;
    const charT* last_pos = nullptr;
    for (; *p != '\0'; ++p) {
        if (*p == '/' || *p == '\\') {
            last_pos = p;
        }
    }

    return last_pos ? last_pos + 1 : file_path;
}

PathString GetDefaultLogName()
{
    PathChar exe_path[MAX_PATH];
    GetModuleFileName(nullptr, exe_path, MAX_PATH);
    
    PathString default_name = ExtractFileName(exe_path);
    default_name.append(L".").append(kFileNameSuffix);

    return default_name;
}

bool InitLogFile()
{
    return true;
}

}   // namespace

LoggingSettings::LoggingSettings()
 : log_item_options(LogItemOptions::ENABLE_TIMESTAMP),
   logging_dest(LoggingDestination::LOG_TO_FILE),
   old_file_option(OldFileOption::APPEND_TO_OLD_LOG_FILE)
{}

void InitLoggingSettings(const LoggingSettings& settings)
{
    log_item_options = settings.log_item_options;
    logging_dest = settings.logging_dest;
    old_file_option = settings.old_file_option;
}

void GetCurrentLoggingSettings(LoggingSettings* settings)
{
    settings->log_item_options = log_item_options;
    settings->logging_dest = logging_dest;
    settings->old_file_option = old_file_option;
}

LogMessage::LogMessage(const char* file, int line, LogSeverity severity)
 : file_(file), line_(line), severity_(severity)
{
    Init(file, line);
}

LogMessage::LogMessage(const char* file, int line)
 : LogMessage(file, line, LOG_INFO) 
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
    
    const char* file_name = ExtractFileName(file);

    stream_ << ':' << file_name << '(' << line << ")]";
    message_start_ = static_cast<size_t>(stream_.tellp());
}

}   // namespace kbase