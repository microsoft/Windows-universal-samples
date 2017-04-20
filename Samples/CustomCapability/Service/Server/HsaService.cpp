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
#include <stdio.h>
#include <windows.h>
#include "ServiceInstaller.h"
#include "ServiceBase.h"
#include "SampleService.h"
#pragma endregion

// 
// Settings of the service
// 

// Internal name of the service
#define SERVICE_NAME             L"HsaService"

// Displayed name of the service
#define SERVICE_DISPLAY_NAME     L"Hsa Sample Service"

// Service start options.
#define SERVICE_START_TYPE       SERVICE_DEMAND_START

// List of service dependencies - "dep1\0dep2\0\0"
#define SERVICE_DEPENDENCIES     L""

// The name of the account under which the service should run - NULL uses LocalSystem account.
#define SERVICE_ACCOUNT          NULL

// The password to the service account name
#define SERVICE_PASSWORD         NULL


//
//  FUNCTION: wmain(int, wchar_t *[])
//
//  PURPOSE: entrypoint for the application.
// 
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    wmain() either performs the command line task, or run the service.
//
int wmain(_In_ int argc, _In_ wchar_t *argv[])
{
    bool invalidArgs = false;

    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
    {
        if (_wcsicmp(L"install", argv[1] + 1) == 0)
        {
            // Install the service when the command is 
            // "-install" or "/install".
            InstallService(
                SERVICE_NAME,               // Name of service
                SERVICE_DISPLAY_NAME,       // Name to display
                SERVICE_START_TYPE,         // Service start type
                SERVICE_DEPENDENCIES,       // Dependencies
                SERVICE_ACCOUNT,            // Service running account
                SERVICE_PASSWORD            // Password of the account
            );
        }
        else if (_wcsicmp(L"remove", argv[1] + 1) == 0)
        {
            // Uninstall the service when the command is 
            // "-remove" or "/remove".
            UninstallService(SERVICE_NAME);
        }
        else if (_wcsicmp(L"console", argv[1] + 1) == 0)
        {
            // Uninstall the service when the command is 
            // "-remove" or "/remove".
            CSampleService service(SERVICE_NAME);
            service.ConsoleRun();
        }
        else
        {
            invalidArgs = true;
        }
    }
    else
    {
        invalidArgs = true;
        CSampleService service(SERVICE_NAME);
        if (!CServiceBase::Run(service))
        {
            wprintf(L"Service failed to run w/err 0x%08lx\n", GetLastError());
        }
    }

    if (invalidArgs)
    {
        wprintf(L"Parameters:\n");
        wprintf(L" -install  to install the service.\n");
        wprintf(L" -remove   to remove the service.\n");
        wprintf(L" -console  to run in console mode.\n");
    }

    return 0;
}