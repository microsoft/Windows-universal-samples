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
    return L"Date and time formatting C++/WinRT sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Format date and time using long and short", xaml_typename<SDKTemplate::Scenario1_LongAndShortFormats>() },
    Scenario{ L"Format using a string template", xaml_typename<SDKTemplate::Scenario2_StringTemplate>() },
    Scenario{ L"Format using a parameterized template", xaml_typename<SDKTemplate::Scenario3_ParameterizedTemplate>() },
    Scenario{ L"Override the current user's settings", xaml_typename<SDKTemplate::Scenario4_Override>() },
    Scenario{ L"Format using Unicode extensions", xaml_typename<SDKTemplate::Scenario5_UnicodeExtensions>() },
    Scenario{ L"Format using different time zones", xaml_typename<SDKTemplate::Scenario6_TimeZone>() },
});

hstring SDKTemplate::StringJoin(std::wstring const& delimiter, IVectorView<hstring> const& vector)
{
    std::wostringstream result;

    bool first = true;
    for (hstring value : vector)
    {
        if (!first)
        {
            result << delimiter;
        }
        result << std::wstring_view(value);
    }
    return hstring(result.str());
}
