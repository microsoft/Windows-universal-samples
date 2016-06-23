// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Device Picker Common Control", "SDKTemplate.Scenario1" },
    { "Enumerate and Watch Devices", "SDKTemplate.Scenario2" },
    { "Enumerate and Watch Devices in a Background Task", "SDKTemplate.Scenario3" },
    { "Enumerate Snapshot of Devices", "SDKTemplate.Scenario4" },
    { "Get Single Device", "SDKTemplate.Scenario5" },
    { "Custom Filter with Additional Properties", "SDKTemplate.Scenario6" },
    { "Request Specific DeviceInformationKind", "SDKTemplate.Scenario7" },
    { "Basic Device Pairing", "SDKTemplate.Scenario8" },
    { "Custom Device Pairing", "SDKTemplate.Scenario9" }
};
