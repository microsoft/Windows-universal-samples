// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Device Picker Common Control", "DeviceEnumerationCpp.Scenario1" },
    { "Enumerate and Watch Devices", "DeviceEnumerationCpp.Scenario2" },
    { "Enumerate and Watch Devices in a Background Task", "DeviceEnumerationCpp.Scenario3" },
    { "Enumerate Snapshot of Devices", "DeviceEnumerationCpp.Scenario4" },
    { "Get Single Device", "DeviceEnumerationCpp.Scenario5" },
    { "Adv: Custom Filter with Additional Properties", "DeviceEnumerationCpp.Scenario6" },
    { "Adv: Request Specific DeviceInformationKind", "DeviceEnumerationCpp.Scenario7" },
    { "Adv: Pair Device", "DeviceEnumerationCpp.Scenario8" }
};
