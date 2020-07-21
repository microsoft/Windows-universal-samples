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
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation::Collections;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Calendar C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
    {
        Scenario{ L"Display a calendar", xaml_typename<SDKTemplate::Scenario1_Data>() },
        Scenario{ L"Retrieve calendar statistics", xaml_typename<SDKTemplate::Scenario2_Stats>() },
        Scenario{ L"Calendar enumeration and math", xaml_typename<SDKTemplate::Scenario3_Enum>() },
        Scenario{ L"Calendar with Unicode extensions in languages", xaml_typename<SDKTemplate::Scenario4_UnicodeExtensions>() },
        Scenario{ L"Calendar time zone support", xaml_typename<SDKTemplate::Scenario5_TimeZone>() },
    });
