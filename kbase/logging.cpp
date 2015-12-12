/*
 @ 0xCCCCCCCC
*/

#include "kbase\logging.h"

#include <Windows.h>

#include <cstdio>
#include <ctime>
#include <iomanip>
#include <stdexcept>

#include "kbase\scoped_handle.h"

namespace {

using kbase::LogSeverity;
using kbase::LogItemOptions;
using kbase::LoggingDestination;
using kbase::OldFileDisposalOption;

using kbase::PathChar;
using kbase::PathString;
using kbase::ScopedSysHandle;

const char* kLogSeverityNames[] {"INFO", "WARNING", "ERROR", "FATAL"};

const LogSeverity kAlwaysPrintErrorMinLevel = LogSeverity::LOG_ERROR;

LogSeverity g_min_severity_level = LogSeverity::LOG_INFO;
LogItemOptions g_log_item_options = LogItemOptions::ENABLE_TIMESTAMP;
LoggingDestination g_logging_dest = LoggingDestination::LOG_TO_FILE;
OldFileDisposalOption g_old_file_option = OldFileDisposalOption::APPEND_TO_OLD_LOG_FILE;

PathString g_log_file_path;
ScopedSysHandle g_log_file;

const PathChar kLogFileName[] = L"_debug_message.log";

template<typename E>
constexpr auto ToUnderlying(E e)
{
    return static_cast<std::underlying_type_t<E>>(e);
}

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

// Returns the default path for log file.
// We use the same path as the EXE file.
PathString GetDefaultLogFilePath()
{
    const size_t kMaxPath = MAX_PATH + 1;
    PathChar exe_path[kMaxPath];
    GetModuleFileNameW(nullptr, exe_path, kMaxPath);

    const PathChar kExeExt[] = L".exe";
    PathString default_path(exe_path);
    auto dot_pos = default_path.rfind(kExeExt);
    default_path.replace(dot_pos, _countof(kExeExt) - 1, kLogFileName);

    return default_path;
}

// Returns the fallback path for the log file.
// We use the path in current directory as the fallback path, when using the
// default path is not possible.
PathString GetFallbackLogFilePath()
{
    const size_t kMaxPath = MAX_PATH + 1;
    PathChar cur_path[kMaxPath];
    GetCurrentDirectoryW(kMaxPath, cur_path);

    PathString fallback_path(cur_path);
    fallback_path.append(L"\\").append(ExtractFileName(g_log_file_path.c_str()));

    return fallback_path;
}

// Once this function succeed, `g_log_file` refers to a valid and writable file.
// Returns true, if we initialized the log file successfully, false otherwise.
bool InitLogFile()
{
    if (g_log_file_path.empty()) {
        g_log_file_path = GetDefaultLogFilePath();
    }

    if (g_old_file_option == OldFileDisposalOption::DELETE_OLD_LOG_FILE) {
        DeleteFileW(g_log_file_path.c_str());
    }

    // Surprisingly, we need neither a local nor a global lock here, on Windows.
    // Because if we opened a file with `FILE_APPEND_DATA` flag only, the system
    // will ensure that each appending is atomic.
    // See https://msdn.microsoft.com/en-us/library/windows/hardware/ff548289(v=vs.85).aspx.
    g_log_file.Reset(CreateFileW(g_log_file_path.c_str(),
                                 FILE_APPEND_DATA,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 nullptr,
                                 OPEN_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 nullptr));
    if (!g_log_file) {
        g_log_file_path = GetFallbackLogFilePath();
        g_log_file.Reset(CreateFileW(g_log_file_path.c_str(),
                                     FILE_APPEND_DATA,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     nullptr,
                                     OPEN_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL,
                                     nullptr));
    }

    return static_cast<bool>(g_log_file);
}

}   // namespace

namespace kbase {

namespace internal {

LogSeverity GetMinSeverityLevel()
{
    return g_min_severity_level;
}

}   // namespace internal

LoggingSettings::LoggingSettings()
 : min_severity_level(LogSeverity::LOG_INFO),
   log_item_options(LogItemOptions::ENABLE_TIMESTAMP),
   logging_destination(LoggingDestination::LOG_TO_FILE),
   old_file_disposal_option(OldFileDisposalOption::APPEND_TO_OLD_LOG_FILE)
{}

void ConfigureLoggingSettings(const LoggingSettings& settings)
{
    g_min_severity_level = settings.min_severity_level;
    g_log_item_options = settings.log_item_options;
    g_logging_dest = settings.logging_destination;
    g_old_file_option = settings.old_file_disposal_option;

    if (!(g_logging_dest & LoggingDestination::LOG_TO_FILE)) {
        return;
    }

    if (!settings.log_file_path.empty()) {
        g_log_file_path = settings.log_file_path;
    }

    // TODO: consider assert the result value on DEBUG mode.
    InitLogFile();
}

LogMessage::LogMessage(const char* file, int line, LogSeverity severity)
 : file_(file), line_(line), severity_(severity)
{
    Init(file, line);
}

LogMessage::LogMessage(const char* file, int line)
 : LogMessage(file, line, LogSeverity::LOG_INFO)
{}

LogMessage::~LogMessage()
{
    if (severity_ == LogSeverity::LOG_FATAL) {
        // TODO: logging stack trace.
    }

    stream_ << std::endl;
    std::string msg = stream_.str();

    if (g_logging_dest & LoggingDestination::LOG_TO_SYSTEM_DEBUG_LOG) {
        OutputDebugStringA(msg.c_str());
        // Also writes to standard error stream.
        fwrite(msg.data(), sizeof(char), msg.length(), stderr);
        fflush(stderr);
    } else if (severity_ >= kAlwaysPrintErrorMinLevel) {
        fwrite(msg.data(), sizeof(char), msg.length(), stderr);
        fflush(stderr);
    }

    // If unfortunately, we failed to initialize the log file, just skip the writting.
    if (g_logging_dest & LoggingDestination::LOG_TO_FILE) {
        if (g_log_file) {
            DWORD bytes_written = 0;
            WriteFile(g_log_file, msg.data(), static_cast<DWORD>(msg.length() * sizeof(char)),
                      &bytes_written, nullptr);
        }
    }
}

void LogMessage::Init(const char* file, int line)
{
    stream_ << "[";

    if (g_log_item_options & LogItemOptions::ENABLE_PROCESS_ID) {
        stream_ << GetCurrentProcessId() << ":";
    }

    if (g_log_item_options & LogItemOptions::ENABLE_THREAD_ID) {
        stream_ << GetCurrentThreadId() << ":";
    }

    if (g_log_item_options & LogItemOptions::ENABLE_TIMESTAMP) {
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

    stream_ << kLogSeverityNames[ToUnderlying(severity_)];

    const char* file_name = ExtractFileName(file);

    stream_ << ':' << file_name << '(' << line << ")]";
}

}   // namespace kbase