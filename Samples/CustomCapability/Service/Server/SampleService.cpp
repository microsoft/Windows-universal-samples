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

#pragma region Includes
#include "SampleService.h"
#include "RpcServer.h"
#include <Evntrace.h>
#pragma endregion

CSampleService::CSampleService(
    PWSTR pszServiceName,
    BOOL fCanStop,
    BOOL fCanShutdown,
    BOOL fCanPauseContinue)
    : CServiceBase(pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue)
{
}

void CSampleService::WriteEventLogEntry(PWSTR pszMessage, BYTE bLevel)
{
    if (IsConsoleRun())
    {
        wprintf(L"%d: %ls\n", bLevel, pszMessage);
    }
    __super::WriteEventLogEntry(pszMessage, bLevel);
}

CSampleService::~CSampleService(void)
{
}

//
// This is the thread pool work callback function.
//
VOID CALLBACK ServiceWorkerThread(
    _In_ PTP_CALLBACK_INSTANCE /*Instance*/,
    _In_ PVOID Parameter,
    _In_ PTP_WORK /*Work*/)
{
    //
    // Do something when the work callback is invoked.
    //
    {
        _int64 status = RpcServerStart();
        if (status)
        {
            CSampleService* sampleService = static_cast<CSampleService *>(Parameter);
            sampleService->Stop();
        }
    }

    return;
}

//
//   FUNCTION: CSampleService::OnStart(DWORD, LPWSTR *)
//
//   PURPOSE: The function is executed when a Start command is sent to the 
//   service by the SCM or when the operating system starts (for a service 
//   that starts automatically). It specifies actions to take when the 
//   service starts. In this code sample, OnStart logs a service-start 
//   message to the Application log, and queues the main service function for 
//   execution in a thread pool worker thread.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
//
//   NOTE: A service application is designed to be long running. Therefore, 
//   it usually polls or monitors something in the system. The monitoring is 
//   set up in the OnStart method. However, OnStart does not actually do the 
//   monitoring. The OnStart method must return to the operating system after 
//   the service's operation has begun. It must not loop forever or block. To 
//   set up a simple monitoring mechanism, one general solution is to create 
//   a timer in OnStart. The timer would then raise events in your code 
//   periodically, at which time your service could do its monitoring. The 
//   other solution is to spawn a new thread to perform the main service 
//   functions, which is demonstrated in this code sample.
//
void CSampleService::OnStart(
    _In_ DWORD dwArgc,
    _In_ LPWSTR *lpszArgv)
{
    // Log a service start message to the Application log.
    WriteEventLogEntry(L"CppWindowsService in OnStart", TRACE_LEVEL_INFORMATION);

    // Queue the main service function for execution in a worker thread.
    PTP_WORK_CALLBACK workcallback = ServiceWorkerThread;
    m_work = CreateThreadpoolWork(workcallback, this, nullptr);

    if (NULL == m_work)
    {
        // TODO: Capture get last error
        WriteEventLogEntry(L"CreateThreadpoolWork failed", TRACE_LEVEL_ERROR);
    }

    //
    // Submit the work to the pool. Because this was a pre-allocated
    // work item (using CreateThreadpoolWork), it is guaranteed to execute.
    //
    SubmitThreadpoolWork(m_work);
}

//
//   FUNCTION: CSampleService::ConsoleRun()
//
//   PURPOSE: The function is executed to simulate OnStart in
//   console mode.
//
void CSampleService::ConsoleRun()
{
    m_runningInConsole = true;

    WriteEventLogEntry(L"Starting Rpc Server..", TRACE_LEVEL_INFORMATION);
    long status = RpcServerStart();

    if (status)
    {
        printf_s("RpcServerConnect returned: 0x%x\n", status);
        WriteEventLogEntry(L"Starting Rpc Server..", TRACE_LEVEL_INFORMATION);
        exit(static_cast<int>(status));
    }
}

//
//   FUNCTION: CSampleService::OnStop()
//
//   PURPOSE: The function is executed when a Stop command is sent to the 
//   service by SCM. It specifies actions to take when a service stops 
//   running. In this code sample, OnStop logs a service-stop message to the 
//   Application log, and waits for the finish of the main service function.
//
//   COMMENTS:
//   Be sure to periodically call ReportServiceStatus() with 
//   SERVICE_STOP_PENDING if the procedure is going to take long time. 
//
void CSampleService::OnStop()
{
    // Log a service stop message to the Application log.
    WriteEventLogEntry(L"CppWindowsService in OnStop", TRACE_LEVEL_INFORMATION);

    // Instruct server to stop listening to remote procedure calls and 
    // unregister rpc interface
    RpcServerDisconnect();
}