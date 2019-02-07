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
using namespace SDKTemplate;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"BluetoothLE C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Client: Discover servers", xaml_typename<SDKTemplate::Scenario1_Discovery>() },
    Scenario{ L"Client: Connect to a server", xaml_typename<SDKTemplate::Scenario2_Client>() },
    Scenario{ L"Server: Publish foreground", xaml_typename<SDKTemplate::Scenario3_ServerForeground>() },
});

hstring SampleState::SelectedBleDeviceId;
hstring SampleState::SelectedBleDeviceName{ L"No device selected" };

Rect winrt::SDKTemplate::GetElementRect(FrameworkElement const& element)
{
    auto transform = element.TransformToVisual(nullptr);
    Point point = transform.TransformPoint({});
    return { point, { static_cast<float>(element.ActualWidth()), static_cast<float>(element.ActualHeight()) } };
}
