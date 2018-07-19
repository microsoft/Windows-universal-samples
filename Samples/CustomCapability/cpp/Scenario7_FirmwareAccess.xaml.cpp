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
#include "Scenario7_FirmwareAccess.xaml.h"
#include <string>
#include "firmwareaccess.h"
#include <intsafe.h>

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;

FirmwareAccess::FirmwareAccess()
{
    InitializeComponent();
}

void FirmwareAccess::GetManufacturerName_Click(Object^ sender, RoutedEventArgs^ e)
{
    wchar_t name[MANUFACTURER_NAME_LENGTH_MAX] = { 0 };

    DWORD error = GetManufacturerNameFromSmbios(name, ARRAYSIZE(name));
    if (error == ERROR_SUCCESS)
    {
        ManufacturerName->Text = ref new String(name);
    }
    else
    {
        rootPage->NotifyUser("Failed to retrieve manufacturer name from SMBIOS, error " + error.ToString(), NotifyType::ErrorMessage);
    }
}

void FirmwareAccess::SecureBootState_Click(Object^ sender, RoutedEventArgs^ e)
{
    bool secureBootEnabled = false;

    DWORD error = GetSecureBootEnabledFromUefi(&secureBootEnabled);
    switch (error)
    {
    case ERROR_SUCCESS:
        SecureBootState->Text = secureBootEnabled.ToString();
        break;

    case ERROR_INVALID_FUNCTION:
        rootPage->NotifyUser("Windows was installed using legacy BIOS", NotifyType::ErrorMessage);
        break;

    default:
        rootPage->NotifyUser("Failed to retrieve secure boot state from UEFI, error " + error.ToString(), NotifyType::ErrorMessage);
        break;
    }
}
