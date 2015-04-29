// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Load images for different scales",         "SDKTemplate.Scenario1" },
    { "Override default scaling of UI elements",  "SDKTemplate.Scenario2" },
};
