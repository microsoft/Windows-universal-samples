// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Scenario 1 Conversion", "SDKTemplate.Scenario1_Conversion" },
    { "Scenario 2 Prediction", "SDKTemplate.Scenario2_Prediction" },
    { "Scenario 3 ReverseConversion", "SDKTemplate.Scenario3_ReverseConversion" }
};
