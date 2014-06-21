/*
 @ Kingsley Chen
*/

#include "kbase/logging.h"

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <stdexcept>

#include <Windows.h>

namespace kbase {

namespace {

const char* log_severity_names[] = {"INFO", "ERROR", "FATAL"};

const LogSeverity kAlwaysPrintErrorLevel = LOG_ERROR;

LogItemOptions log_item_options = LogItemOptions::ENABLE_TIMESTAMP;

LoggingDestination logging_dest = LoggingDestination::LOG_TO_FILE;

OldFileOption old_file_option = OldFileOption::APPEND_TO_OLD_LOG_FILE;

typedef wchar_t PathChar;
typedef std::wstring PathString;

FILE* log_file = nullptr;

const PathChar kLogFileName[] = L"debug_message.log";

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

/*
 @ Returns the path of default log file.
   We use the same path as the EXE file.
*/
PathString GetDefaultLogFile()
{
    PathChar exe_path[MAX_PATH];
    GetModuleFileName(nullptr, exe_path, MAX_PATH);
    
    const PathChar* end_past_slash = ExtractFileName(exe_path);
    PathString default_path(exe_path, end_past_slash);
    default_path.append(kLogFileName);

    return default_path;
}

/*
 @ This function needs an global mutex-like protector that has this call enclosed.
   We can have multiple threads and/or processes, try to prevent messing up each
   other's writes.
 @ returns true, if the file is ready to write
           false, otherwise
*/
bool InitLogFile()
{
    if (old_file_option == OldFileOption::DELETE_OLD_LOG_FILE) {
        if (log_file) {
            fclose(log_file);
            log_file = nullptr;
        }

        PathString&& log_file_name = GetDefaultLogFile();
        _wremove(log_file_name.c_str());
    }
    
    if (log_file) {
        return true;
    }

    PathString&& log_file_name = GetDefaultLogFile();
    log_file = _wfsopen(log_file_name.c_str(), L"a", _SH_DENYNO);

    if (!log_file) {
        return false;
    }

    return true;
}

// A global file-lock wrapper.
// If it fails to create a mutex object, or open the existed one, it throws a
// runtime error exception, and leaves the program crashed by default.
class LoggingLock {
public:
    LoggingLock()
    {
        InitLock();
        Lock();
    }

    ~LoggingLock()
    {
        Unlock();
    }

private:
    LoggingLock(const LoggingLock&) = delete;
    LoggingLock& operator=(const LoggingLock&) = delete;

    void InitLock()
    {
        PathString log_name = GetDefaultLogFile();
        // we want file name to be part of the mutex name, and \ is not a legal
        // character, so we replace \ with /
        std::replace(log_name.begin(), log_name.end(), L'\\', L'/');

        std::wstring mutex_name = L"Global\\";
        mutex_name.append(log_name);

        // if the mutex has alread been created by another thread or process
        // this call returns the handle to the existed mutex
        log_mutex_ = CreateMutex(nullptr, false, mutex_name.c_str());
        if (!log_mutex_) {
            DWORD err = GetLastError();
            throw std::runtime_error("failed to create a mutex object!\nerror code: "
                                     + std::to_string(err));
        }
    }

    void Lock()
    {
        WaitForSingleObject(log_mutex_, INFINITE);
    }

    void Unlock()
    {
        ReleaseMutex(log_mutex_);
        CloseHandle(log_mutex_);
    }

private:
    typedef HANDLE MutexHandle;
    MutexHandle log_mutex_ = nullptr;
};

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
#if _DEBUG
    if (severity_ == LOG_FATAL) {
        // TODO: log stack trace information
    }
#endif

    stream_ << std::endl;
    std::string msg = stream_.str();

    if ((logging_dest & LoggingDestination::LOG_TO_SYSTEM_DEBUG_LOG) ||
        (severity_ >= kAlwaysPrintErrorLevel)) {
        OutputDebugStringA(msg.c_str());
        fprintf_s(stderr, "%s", msg.c_str());
        fflush(stderr);
    }

    if (logging_dest & LoggingDestination::LOG_TO_FILE) {
        LoggingLock lock;
        if (InitLogFile()) {
            fwrite(static_cast<const void*>(msg.c_str()), sizeof(char), msg.length(),
                   log_file);
            fflush(log_file);
        }
    }

    if (severity_ == LOG_FATAL) {
        throw std::runtime_error("encountered a fatal error!");
    }
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
}

}   // namespace kbase