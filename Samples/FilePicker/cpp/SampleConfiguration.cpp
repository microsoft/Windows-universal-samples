// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Pick a single photo",   "SDKTemplate.Scenario1_SingleFile" },
    { "Pick multiple files",   "SDKTemplate.Scenario2_MultiFile" },
    { "Pick a folder",         "SDKTemplate.Scenario3_SingleFolder" },
    { "Save a file",           "SDKTemplate.Scenario4_SaveFile" },
    { "Trigger CFU",           "SDKTemplate.Scenario5_TriggerCFU" }
};
