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
    { "Display a calendar", "SDKTemplate.Scenario1_Data" },
    { "Retrieve calendar statistics", "SDKTemplate.Scenario2_Stats" },
    { "Calendar enumeration and math", "SDKTemplate.Scenario3_Enum" },
    { "Calendar with Unicode extensions in languages", "SDKTemplate.Scenario4_UnicodeExtensions" },
    { "Calendar time zone support", "SDKTemplate.Scenario5_TimeZone" }
};
