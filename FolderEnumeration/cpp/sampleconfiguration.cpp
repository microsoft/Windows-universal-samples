// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Enumerate files and folders in the Pictures library",        "SDKTemplate.Scenario1" },
    { "Enumerate files in the Pictures library, by groups",         "SDKTemplate.Scenario2" },
    { "Enumerate files in the Pictures library with prefetch APIs", "SDKTemplate.Scenario3" },
    { "Enumerate files in a folder and display availability",       "SDKTemplate.Scenario4" },
};
