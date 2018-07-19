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

#include "pch.h"
#include "firmwareaccess.h"

#define EFI_GLOBAL_VAR_GUID		L"{8BE4DF61-93CA-11D2-AA0D-00E098032B8C}"

DWORD
GetSecureBootEnabledFromUefi(
    _Out_ bool* SecureBootEnabled
    )
{
    BYTE secureBootStatus;
    DWORD bytesReturned;
    DWORD error = ERROR_SUCCESS;
    bytesReturned = GetFirmwareEnvironmentVariable(L"SecureBoot",
                                                   EFI_GLOBAL_VAR_GUID,
                                                   &secureBootStatus,
                                                   sizeof(BYTE));
    if (bytesReturned <= 0)
    {
        error = GetLastError();
        goto exit;
    }

    if (bytesReturned != sizeof(BYTE))
    {
        error = ERROR_INVALID_DATA;
        goto exit;
    }

    *SecureBootEnabled = (secureBootStatus != 0);

exit:
    return error;
}