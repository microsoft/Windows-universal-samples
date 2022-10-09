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
    return L"ApplicationData C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Files", xaml_typename<SDKTemplate::Scenario1_Files>() },
    Scenario{ L"Settings", xaml_typename<SDKTemplate::Scenario2_Settings>() },
    Scenario{ L"Setting Containers", xaml_typename<SDKTemplate::Scenario3_SettingContainer>() },
    Scenario{ L"Composite Settings", xaml_typename<SDKTemplate::Scenario4_CompositeSettings>() },
    Scenario{ L"ms-appdata:// Protocol", xaml_typename<SDKTemplate::Scenario5_Msappdata>() },
    Scenario{ L"Clear", xaml_typename<SDKTemplate::Scenario6_ClearScenario>() },
    Scenario{ L"SetVersion", xaml_typename<SDKTemplate::Scenario7_SetVersion>() },
});
