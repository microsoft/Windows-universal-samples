// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Device Picker Common Control", "SDKTemplate.Scenario1_DevicePicker" },
    { "Enumerate and Watch Devices", "SDKTemplate.Scenario2_DeviceWatcher" },
    { "Enumerate and Watch Devices in a Background Task", "SDKTemplate.Scenario3_BackgroundDeviceWatcher" },
    { "Enumerate Snapshot of Devices", "SDKTemplate.Scenario4_Snapshot" },
    { "Get Single Device", "SDKTemplate.Scenario5_GetSingleDevice" },
    { "Custom Filter with Additional Properties", "SDKTemplate.Scenario6_CustomFilterAddedProps" },
    { "Request Specific DeviceInformationKind", "SDKTemplate.Scenario7_DeviceInformationKind" },
    { "Basic Device Pairing", "SDKTemplate.Scenario8_PairDevice" },
    { "Custom Device Pairing", "SDKTemplate.Scenario9_CustomPairDevice" }
};
