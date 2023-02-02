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
using namespace winrt::SDKTemplate;
using namespace winrt::Windows::Foundation::Collections;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Windows Audio Session API (WASAPI) Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Device Enumeration", xaml_typename <SDKTemplate::Scenario1>() },
    Scenario{ L"Audio Rendering with Hardware Offload", xaml_typename<SDKTemplate::Scenario2>() },
    Scenario{ L"Audio Rendering with Low Latency", xaml_typename<SDKTemplate::Scenario3>() },
    Scenario{ L"PCM Audio Capture", xaml_typename<SDKTemplate::Scenario4>() },
});
