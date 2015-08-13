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
    { "Format date and time using long and short", "SDKTemplate.Scenario1_LongAndShortFormats" },
    { "Format using a string template", "SDKTemplate.Scenario2_StringTemplate" },
    { "Format using a parameterized template", "SDKTemplate.Scenario3_ParameterizedTemplate" },
    { "Override the current user's settings", "SDKTemplate.Scenario4_Override" },
    { "Format using Unicode extensions", "SDKTemplate.Scenario5_UnicodeExtensions" },
    { "Format using different time zones", "SDKTemplate.Scenario6_TimeZone" }
}; 
