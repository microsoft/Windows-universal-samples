//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    // The format here is the following:
    //     { "Description for the sample", "Fully quaified name for the class that implements the scenario" }
    { "String Resources In XAML", "SDKTemplate.Scenario1" },
    { "File Resources In XAML", "SDKTemplate.Scenario2" },
    { "String Resources In Code", "SDKTemplate.Scenario3" },
    { "Resources in the AppX manifest", "SDKTemplate.Scenario4" },
    { "Additional Resource Files", "SDKTemplate.Scenario5" },
    { "Runtime Changes/Events", "SDKTemplate.Scenario7" },
    { "Application Languages", "SDKTemplate.Scenario8" },
    { "Multi-dimensional fallback", "SDKTemplate.Scenario10" },
    { "Working with webservices", "SDKTemplate.Scenario11" },
    { "Retrieving resources in non-UI threads", "SDKTemplate.Scenario12" },
    { "File resources in code", "SDKTemplate.Scenario13" }
};
