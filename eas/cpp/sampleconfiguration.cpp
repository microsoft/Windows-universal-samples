// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Get Device Information", "EAS.Scenario1_GetDeviceInformation" },
    { "Check Compliance", "EAS.Scenario2_CheckCompliance" },
    { "Apply EAS Policy", "EAS.Scenario3_ApplyEASPolicy" }
};
