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

// Provides a sample service class that derives from the service base class -
// CServiceBase. The sample service logs the service start and stop
// information to the Application event log, and shows how to run the main
// function of the service in a thread pool worker thread.

#pragma once

#include "ServiceBase.h"

class CSampleService : public CServiceBase
{
public:

    CSampleService(PWSTR pszServiceName,
        BOOL fCanStop = TRUE,
        BOOL fCanShutdown = TRUE,
        BOOL fCanPauseContinue = FALSE);
    void ConsoleRun();
    bool IsConsoleRun()
    {
        return m_runningInConsole;
    }
    void WriteEventLogEntry(PWSTR pszMessage, BYTE bLevel) override;
    virtual ~CSampleService(void);

protected:

    virtual void OnStart(DWORD dwArgc, PWSTR *pszArgv) override;
    virtual void OnStop() override;


private:
    PTP_WORK m_work = nullptr;
    bool m_runningInConsole = false;
};