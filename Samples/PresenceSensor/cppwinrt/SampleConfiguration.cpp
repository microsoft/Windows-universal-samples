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
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation::Collections;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Presence Sensor";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Data Events", xaml_typename<SDKTemplate::Scenario1_DataEvents>() },
    Scenario{ L"Polling", xaml_typename<SDKTemplate::Scenario2_Polling>() },
    Scenario{ L"Choosing", xaml_typename<SDKTemplate::Scenario3_Choosing>() },
});

hstring winrt::to_hstring(HumanPresence value)
{
    switch (value)
    {
    case HumanPresence::Unknown:
        return L"Unknown";
    case HumanPresence::Present:
        return L"Present";
    case HumanPresence::NotPresent:
        return L"NotPresent";
    }
    return to_hstring(static_cast<uint32_t>(value));
}

hstring winrt::to_hstring(HumanEngagement value)
{
    switch (value)
    {
    case HumanEngagement::Unknown:
        return L"Unknown";
    case HumanEngagement::Engaged:
        return L"Engaged";
    case HumanEngagement::Unengaged:
        return L"Unengaged";
    }
    return to_hstring(static_cast<uint32_t>(value));
}
