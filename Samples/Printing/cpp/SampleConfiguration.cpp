// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Basic", "SDKTemplate.Scenario1Basic" },
    { "Standard Options", "SDKTemplate.Scenario2StandardOptions" },
    { "Custom Options", "SDKTemplate.Scenario3CustomOptions" },
    { "Page Range", "SDKTemplate.Scenario4PageRange" },
    { "Photos", "SDKTemplate.Scenario5Photos" },
    { "Disable Preview", "SDKTemplate.Scenario6DisablePreview" }
};
