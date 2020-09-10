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
#include "SampleConfiguration.h"
#include "MainPage.h"
#include <winrt/SDKTemplate.h>

using namespace winrt;
using namespace winrt::SDKTemplate::implementation;

winrt::hstring
MainPage::FEATURE_NAME()
{
    return L"Logging C++/WinRT Sample";
}

Windows::Foundation::Collections::IVector<SDKTemplate::Scenario>
MainPage::scenariosInner = winrt::single_threaded_observable_vector<SDKTemplate::Scenario>(
{
    Scenario{ L"LoggingChannel", winrt::xaml_typename<SDKTemplate::Scenario1>() },
    Scenario{ L"LoggingSession", winrt::xaml_typename<SDKTemplate::Scenario2>() },
    Scenario{ L"FileLoggingSession", winrt::xaml_typename<SDKTemplate::Scenario3>() },
});
