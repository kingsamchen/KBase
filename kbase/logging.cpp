/*
 @ Kingsley Chen
*/

#include "kbase\logging.h"

#include <Windows.h>

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <memory>
#include <mutex>
#include <stdexcept>

#include "kbase\basic_types.h"
#include "kbase\error_exception_util.h"
#include "kbase\memory\scoped_handle.h"

namespace {

using kbase::LogSeverity;
using kbase::LogItemOptions;
using kbase::LoggingDestination;
using kbase::LoggingLockOption;
using kbase::OldFileOption;

using kbase::PathChar;
using kbase::PathString;
using kbase::ScopedStdioHandle;

typedef kbase::ScopedSysHandle GlobalLockHandle;
typedef std::unique_ptr<std::unique_lock<std::mutex>> ThreadLockHandle;

const char* kLogSeverityNames[] {"INFO", "WARNING", "FATAL"};

const LogSeverity kAlwaysPrintErrorLevel = kbase::LOG_WARNING;

LogItemOptions log_item_options = LogItemOptions::ENABLE_TIMESTAMP;

LoggingDestination logging_dest = LoggingDestination::LOG_TO_FILE;

OldFileOption old_file_option = OldFileOption::APPEND_TO_OLD_LOG_FILE;

LoggingLockOption logging_lock_option = LoggingLockOption::USE_GLOBAL_LOCK;

ScopedStdioHandle log_file;

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

// Returns the path of default log file. 
// We use the same path as the EXE file.
PathString GetDefaultLogFile()
{
    PathChar exe_path[MAX_PATH];
    GetModuleFileName(nullptr, exe_path, MAX_PATH);

    const PathChar* end_past_slash = ExtractFileName(exe_path);
    PathString default_path(exe_path, end_past_slash);
    default_path.append(kLogFileName);

    return default_path;
}

// This function needs an global mutex-like protector that has this call enclosed.
// We can have multiple threads and/or processes, try to prevent messing up each
// other's writes.
// Returns true, if the file is ready to write; return false, otherwise.
bool InitLogFile()
{
    if (old_file_option == OldFileOption::DELETE_OLD_LOG_FILE) {
        if (log_file) {
            log_file = nullptr;
        }

        PathString&& log_file_name = GetDefaultLogFile();
        _wremove(log_file_name.c_str());
    }

    if (log_file) {
        return true;
    }

    PathString&& log_file_name = GetDefaultLogFile();
    log_file.Reset(_wfsopen(log_file_name.c_str(), L"a", _SH_DENYNO));

    if (!log_file) {
        return false;
    }

    return true;
}

// A well-tailored lock for resolving race-condition when multiple threads or even
// multiple processes are logging to the file simultanesouly.
class LoggingLock {
public:
    LoggingLock()
    {
        Lock();
    }

    ~LoggingLock()
    {
        Unlock();
    }

    static void InitLock()
    {
        if (initialized_) {
            return;
        }

        if (logging_lock_option == LoggingLockOption::USE_GLOBAL_LOCK) {
            PathString log_name = GetDefaultLogFile();
            // We want the file name to be part of the mutex name, and \ is not a 
            // legal character, so we replace \ with /.
            std::replace(log_name.begin(), log_name.end(), L'\\', L'/');

            std::wstring mutex_name = L"Global\\";
            mutex_name.append(log_name);

            // if the mutex has alread been created by another thread or process
            // this call returns the handle to the existed mutex
            global_lock_.Reset(CreateMutexW(nullptr, false, mutex_name.c_str()));
            ThrowLastErrorIf(!global_lock_, "failed to create a mutex object!");
        } else {
            local_lock_.reset(
                new std::unique_lock<std::mutex>(local_mutex_, std::defer_lock));
        }

        initialized_ = true;
    }

private:
    LoggingLock(const LoggingLock&) = delete;
    LoggingLock& operator=(const LoggingLock&) = delete;

    void Lock()
    {
        if (logging_lock_option == LoggingLockOption::USE_GLOBAL_LOCK) {
            WaitForSingleObject(global_lock_, INFINITE);
        } else {
            local_lock_->lock();
        }
    }

    void Unlock()
    {
        if (logging_lock_option == LoggingLockOption::USE_GLOBAL_LOCK) {
            ReleaseMutex(global_lock_);
        } else {
            local_lock_->unlock();
        }
    }

private:
    static bool initialized_;
    static GlobalLockHandle global_lock_;
    static ThreadLockHandle local_lock_;
    static std::mutex local_mutex_;
};

bool LoggingLock::initialized_ = false;
GlobalLockHandle LoggingLock::global_lock_;
ThreadLockHandle LoggingLock::local_lock_;
std::mutex LoggingLock::local_mutex_;

}   // namespace

namespace kbase {

LoggingSettings::LoggingSettings()
 : log_item_options(LogItemOptions::ENABLE_TIMESTAMP),
   logging_dest(LoggingDestination::LOG_TO_FILE),
   old_file_option(OldFileOption::APPEND_TO_OLD_LOG_FILE),
   logging_lock_option(LoggingLockOption::USE_GLOBAL_LOCK)
{}

void InitLoggingSettings(const LoggingSettings& settings)
{
    log_item_options = settings.log_item_options;
    logging_dest = settings.logging_dest;
    old_file_option = settings.old_file_option;
    logging_lock_option = settings.logging_lock_option;
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
        LoggingLock::InitLock();
        LoggingLock lock;
        if (InitLogFile()) {
            fwrite(static_cast<const void*>(msg.c_str()), sizeof(char), msg.length(),
                   log_file);
            fflush(log_file);
        }
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

    stream_ << kLogSeverityNames[severity_];
    
    const char* file_name = ExtractFileName(file);

    stream_ << ':' << file_name << '(' << line << ")]";
}

}   // namespace kbase