/*
 @ Kingsley Chen
*/

#include "stdafx.h"

#include "gtest\gtest.h"
#include "kbase\logging.h"

#include <Windows.h>

#include <iostream>
#include <thread>
#include <vector>

#define _USE_LOCAL_LOCK_

using namespace kbase;

namespace {

HANDLE action_event = nullptr;

void ThreadFn(int id)
{
    WaitForSingleObject(action_event, INFINITE);
    std::cout << "thread " << id << std::endl;
    DLOG(INFO) << "The thread " << id << " is currently running";
}

}   // namespace

TEST(LoggingTest, MT)
{
    LoggingSettings settings;
#ifdef _USE_LOCAL_LOCK_
    settings.logging_lock_option = LoggingLockOption::USE_LOCAL_LOCK;
#endif
    InitLoggingSettings(settings);
    
    action_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    std::vector<std::thread> vth;
    for (int i = 0; i < 5; ++i) {
        vth.emplace_back(ThreadFn, i);
        vth.back().detach();
    }
    
    std::cout << "all are prepared" << std::endl;
    SetEvent(action_event);
}