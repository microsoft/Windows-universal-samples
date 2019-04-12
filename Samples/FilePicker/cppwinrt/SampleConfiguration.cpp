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
    return L"File picker C++/WinRT sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Pick a single photo",   xaml_typename<SDKTemplate::Scenario1>() },
    Scenario{ L"Pick multiple files",   xaml_typename<SDKTemplate::Scenario2>() },
    Scenario{ L"Pick a folder",         xaml_typename<SDKTemplate::Scenario3>() },
    Scenario{ L"Save a file",           xaml_typename<SDKTemplate::Scenario4>() },
    Scenario{ L"Trigger CFU",           xaml_typename<SDKTemplate::Scenario5>() },
});
