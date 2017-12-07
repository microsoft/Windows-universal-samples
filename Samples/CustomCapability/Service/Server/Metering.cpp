//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "stdafx.h"
#include "Metering.h"
#include <assert.h>  
#include "RpcInterface_h.h" 

using namespace RpcServer;

//
// Thread pool callback method for metering worker
//
void CALLBACK MeteringWorkerTpCallback(
    _In_ PTP_CALLBACK_INSTANCE /*iTimerInstance*/,
    _In_ PVOID pContext,
    _In_ PTP_TIMER /*pTimer*/)
{
    Metering *metering = static_cast<Metering *>(pContext);

    if (metering != nullptr)
    {
        // Run the metering worker for this instance of metering
        metering->MeteringWorker();
    }
}

//
// ctor for Metering
//
Metering::Metering(
    _In_ __int64 period)
{
    tpTimer = CreateThreadpoolTimer(::MeteringWorkerTpCallback, this, nullptr);
    // TODO: Handle if CreateThreadpoolTimer fails i.e tpTimer == nullptr

    samplePeriod = period;
    event = CreateEvent(
        nullptr,  // default security attributes
        FALSE,    // auto-reset event object
        FALSE,    // initial state is nonsignaled
        nullptr); // unnamed object
}

//
// dtor for Metering
//
Metering::~Metering()
{
    if (tpTimer != nullptr)
    {
        // How to close threadpool timer when there are outstanding callbacks:
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682040(v=vs.85).aspx
        SetThreadpoolTimer(tpTimer, nullptr, 0, 0);
        WaitForThreadpoolTimerCallbacks(tpTimer, true);
        CloseThreadpoolTimer(tpTimer);
        tpTimer = nullptr;
    }
    CloseHandle(event);
}

//
// Set the lowest sample period
//
void Metering::SetSamplePeriod(_In_ __int64 period)
{
    if (period < 1)
    {
        // Don't allow 0 (too fast) or negative numbers.
        period = 1;
    }
    if (period > 1000)
    {
        // Don't let the period be too long, or we will be
        // slow to shut down.
        period = 1000;
    }
    samplePeriod = period;
}

//
// Get last known metering data
//
__int64 Metering::GetMeteringData() const
{
    return _data;
}

//
// Metering worker that updates metering data
//
void Metering::MeteringWorker()
{
    // Get the value from the imaginary driver and update the data
    _data = GetTickCount();
    SetEvent(event);
}

//
// Set the thread poot timer and wait for metering data.
//
void Metering::WaitForMeteringData() const
{
    ULARGE_INTEGER ulDueTime;
    FILETIME FileDueTime;
    ulDueTime.QuadPart = static_cast<ULONGLONG>(-(1 * 10 * 1000 * samplePeriod));
    FileDueTime.dwHighDateTime = ulDueTime.HighPart;
    FileDueTime.dwLowDateTime = ulDueTime.LowPart;

    SetThreadpoolTimer(tpTimer,
                       &FileDueTime,
                       0,
                       0);

    WaitForSingleObject(event, INFINITE);
}

void Metering::StartMetering(
    _In_ __int64 samplePeriod,
    _In_ __int64 context)
{
    stopMeteringRequested = false;
    SetSamplePeriod(samplePeriod);

    while (true)
    {
        WaitForMeteringData();
        if (stopMeteringRequested || ShutdownRequested)
        {
            break;
        }
        MeteringDataEvent(GetMeteringData(), context);
    }
}

void Metering::StopMetering()
{
    stopMeteringRequested = true;
}

