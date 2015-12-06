/*
 @ 0xCCCCCCCC
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\logging.h"
#include <Windows.h>

#include <iostream>
#include <thread>
#include <vector>

#define _USE_LOCAL_LOCK_ 1

using namespace kbase;

namespace {

HANDLE action_event = nullptr;

void ThreadFn(int id)
{
    WaitForSingleObject(action_event, INFINITE);
    std::cout << "thread " << id << std::endl;
    DLOG(INFO) << "The thread " << id << " is currently running";
}

bool Boolean(bool b)
{
    std::cout << "Boolean() is called\n";
    return b;
}

}   // namespace

TEST(LoggingTest, MT)
{
    LoggingSettings settings;
#if _USE_LOCAL_LOCK_
    settings.logging_lock_option = LoggingLockOption::USE_LOCAL_LOCK;
#endif
    ConfigureLoggingSettings(settings);

    action_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    std::vector<std::thread> vth;
    for (int i = 0; i < 5; ++i) {
        vth.emplace_back(ThreadFn, i);
        vth.back().detach();
    }

    std::cout << "all are prepared" << std::endl;
    SetEvent(action_event);
    std::cin.get();
}

TEST(LoggingTest, MinLevelAndConditionalLogging)
{
    LoggingSettings logging_settings;
    logging_settings.min_severity_level = LogSeverity::LOG_ERROR;
    logging_settings.logging_destination = LoggingDestination::LOG_TO_ALL;
    ConfigureLoggingSettings(logging_settings);
    LOG(WARNING) << "LOG(WARNING)";
    LOG_IF(ERROR, Boolean(true)) << "LOG_IF(ERROR, Boolean(true))";
    LOG_IF(ERROR, Boolean(false)) << "LOG_IF(ERROR, Boolean(false))";
    DLOG(INFO) << "DLOG(INFO)";
    DLOG(ERROR) << "DLOG(ERROR)";
    DLOG_IF(FATAL, Boolean(true)) << "DLOG_IF(FATAL, Boolean(true))";
    DLOG_IF(FATAL, Boolean(false)) << "DLOG_IF(FATAL, Boolean(false))";
}