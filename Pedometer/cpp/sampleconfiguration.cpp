// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Events", "PedometerCPP.Scenario1_Events" },
    { "History", "PedometerCPP.Scenario2_History" },
    { "Current step count", "PedometerCPP.Scenario3_CurrentStepCount" }
};
