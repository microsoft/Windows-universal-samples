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
#include "SmbiosRt.h"

using namespace RpcClientRt;
using namespace Platform;

/*
WinRT wrapper component to access RpcClient.dll exports
from JavaScript
*/

SmbiosResult^ Smbios::GetManufacturerName()
{
    wchar_t nameBuffer[MANUFACTURER_NAME_LENGTH_MAX];
    unsigned int errorCode = GetManufacturerNameFromSmbios(nameBuffer, ARRAYSIZE(nameBuffer));
    return ref new SmbiosResult(errorCode, errorCode == ERROR_SUCCESS ? ref new String(nameBuffer) : nullptr);
}

SmbiosResult^ Smbios::GetSecureBootEnabled()
{
    bool enabled = false;
    unsigned int errorCode = GetSecureBootEnabledFromUefi(&enabled);
    return ref new SmbiosResult(errorCode, errorCode == ERROR_SUCCESS ? static_cast<Object^>(enabled) : nullptr);
}

