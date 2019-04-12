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
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation::Collections;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Jump List C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
    {
        Scenario{ L"Respond to being launched", xaml_typename<SDKTemplate::Scenario1_Launched>() },
        Scenario{ L"Remove the jump list", xaml_typename<SDKTemplate::Scenario2_Remove>() },
        Scenario{ L"Creating custom items", xaml_typename<SDKTemplate::Scenario3_CustomItems>() },
        Scenario{ L"Change the system group", xaml_typename<SDKTemplate::Scenario4_ChangeSystemGroup>() },
        Scenario{ L"Check if the platform is supported", xaml_typename<SDKTemplate::Scenario5_IsSupported>() },
    });

void winrt::SDKTemplate::App_LaunchCompleted(LaunchActivatedEventArgs const& e)
{
    hstring arguments = e.Arguments();
    if (!arguments.empty())
    {
        implementation::MainPage::Current().Navigate(xaml_typename<Scenario1_Launched>(), box_value(L"arguments: " + arguments));
    }
}

void winrt::SDKTemplate::App_OnFileActivated(FileActivatedEventArgs const& e)
{
    if (e.Files().Size() == 1)
    {
        implementation::MainPage::Current().Navigate(xaml_typename<Scenario1_Launched>(), box_value(L"file: " + e.Files().GetAt(0).Name()));
    }
}
