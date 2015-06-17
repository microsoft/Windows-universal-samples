//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#pragma once
#include <chrono>
#include <string>
#include <agents.h>

namespace SDKTemplate
{
    // Create a time stamp to append to log file names so 
    // each file has a unique name. The format is:
    //     YYMMDD-hhmmssMMM 
    // where
    //     YY == year
    //     MM == month
    //     DD == day
    //     hh == hours
    //     mm == minutes
    //     ss == seconds
    //     MMM == milliseconds
    inline std::wstring GetTimeStamp()
    {
        std::wstring result;
        SYSTEMTIME timeNow;
        GetLocalTime(&timeNow);
        result.resize(16);
        swprintf_s(
            &result[0],
            result.size() + 1,
            L"%02u%02u%02u-%02u%02u%02u%03u",
            timeNow.wYear - 2000,
            timeNow.wMonth,
            timeNow.wDay,
            timeNow.wHour,
            timeNow.wMinute,
            timeNow.wSecond,
            timeNow.wMilliseconds);
        return result;
    }

    // Create a task which delays for a specified duration before executing a function.
    template <typename Func>
    auto create_delayed_task(
        std::chrono::milliseconds delay,
        Func func, concurrency::cancellation_token token = concurrency::cancellation_token::none())
        -> decltype(create_task(func))
    {
        concurrency::task_completion_event<void> tce;

        auto pTimer = new concurrency::timer<int>(static_cast<int>(delay.count()), 0, NULL, false);
        auto pCallback = new concurrency::call<int>([tce](int)
        {
            tce.set();
        });

        pTimer->link_target(pCallback);
        pTimer->start();

        return create_task(tce).then([pCallback, pTimer]()
        {
            delete pCallback;
            delete pTimer;
        }).then(func, token);
    }
}
