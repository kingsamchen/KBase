/*
 @ 0xCCCCCCCC
*/

#include "kbase/logging.h"

#include <cstdio>
#include <ctime>
#include <iomanip>
#include <thread>

#include <Windows.h>

#include "kbase/error_exception_util.h"
#include "kbase/scoped_handle.h"
#include "kbase/stack_walker.h"

namespace {

using kbase::LogSeverity;
using kbase::LogItemOptions;
using kbase::LoggingDestination;
using kbase::OldFileDisposalOption;

using kbase::PathChar;
using kbase::PathString;
using kbase::ScopedSysHandle;

const char* kLogSeverityNames[] { "INFO", "WARNING", "ERROR", "FATAL" };

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

// Ouputs timestamp in the form like "20160126 09:14:38,456".
void OutputNowTimestamp(std::ostream& stream)
{
    namespace chrono = std::chrono;

    // Because c-style date&time utilities don't support microsecond precison,
    // we have to handle it on our own.
    auto time_now = chrono::system_clock::now();
    auto duration_in_ms = chrono::duration_cast<chrono::milliseconds>(time_now.time_since_epoch());
    auto ms_part = duration_in_ms - chrono::duration_cast<chrono::seconds>(duration_in_ms);

    tm local_time_now;
    time_t raw_time = chrono::system_clock::to_time_t(time_now);
    _localtime64_s(&local_time_now, &raw_time);
    stream << std::put_time(&local_time_now, "%Y%m%d %H:%M:%S,")
           << std::setfill('0') << std::setw(3) << ms_part.count();
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
    ENSURE(CHECK, !g_log_file_path.empty()).Require();

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
    if (g_log_file) {
        return true;
    }

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

    auto rv = InitLogFile();
    ENSURE(CHECK, rv)(LastError()).Require();
}

LogMessage::LogMessage(const char* file, int line, LogSeverity severity)
 : file_(file), line_(line), severity_(severity)
{
    InitMessageHeader();
}

LogMessage::~LogMessage()
{
    if (severity_ == LogSeverity::LOG_FATAL) {
        stream_ << "\n";
        StackWalker walker;
        walker.DumpCallStack(stream_);
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

    // If `InitLogFile` wasn't called at the start of the program, do it on the spot.
    // However, if we unfortunately failed to initialize the log file, just skip the writting.
    // Note that, if more than one thread in here try to call `InitLogFile`, there will be a
    // race condition. This is why you should call `ConfigureLoggingSettings` at start.
    if ((g_logging_dest & LoggingDestination::LOG_TO_FILE) && InitLogFile()) {
        DWORD bytes_written = 0;
        WriteFile(g_log_file, msg.data(), static_cast<DWORD>(msg.length() * sizeof(char)),
                  &bytes_written, nullptr);
    }
}

void LogMessage::InitMessageHeader()
{
    stream_ << "[";

    if (g_log_item_options & LogItemOptions::ENABLE_TIMESTAMP) {
        OutputNowTimestamp(stream_);
    }

    if (g_log_item_options & LogItemOptions::ENABLE_PROCESS_ID) {
        stream_ << " " << GetCurrentProcessId();
    }

    if (g_log_item_options & LogItemOptions::ENABLE_THREAD_ID) {
        stream_ << " " << std::this_thread::get_id();
    }

    stream_ << " " << kLogSeverityNames[ToUnderlying(severity_)]
            << " " << ExtractFileName(file_) << '(' << line_ << ")]";
}

}   // namespace kbase