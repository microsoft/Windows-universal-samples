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
    return L"AppWindow C++/WinRT sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Create and show secondary window", xaml_typename<SDKTemplate::Scenario1_SecondaryWindow>() },
    Scenario{ L"Show window on another display", xaml_typename<SDKTemplate::Scenario2_DisplayRegion>() },
    Scenario{ L"Specific size for secondary window", xaml_typename<SDKTemplate::Scenario3_Size>() },
    Scenario{ L"CompactOverlay secondary window",  xaml_typename < SDKTemplate::Scenario4_CompactOverlay>() },
    Scenario{ L"Position secondary window",  xaml_typename < SDKTemplate::Scenario5_RelativePositioning>() },
});

bool winrt::SDKTemplate::TryParseFloat(hstring const& str, float& result)
{
    errno = 0;
    wchar_t* endptr;
    result = wcstof(str.c_str(), &endptr);
    return errno == 0 && std::isfinite(result);
}
