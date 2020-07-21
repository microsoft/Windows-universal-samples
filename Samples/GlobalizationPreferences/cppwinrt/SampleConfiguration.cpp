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
using namespace Windows::Globalization;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Globalization preferences C++/WinRT sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"User preferences", xaml_typename<SDKTemplate::Scenario1_Prefs>() },
    Scenario{ L"Language characteristics", xaml_typename<SDKTemplate::Scenario2_Lang>() },
    Scenario{ L"Region characteristics", xaml_typename<SDKTemplate::Scenario3_Region>() },
    Scenario{ L"Current input language", xaml_typename<SDKTemplate::Scenario4_Input>() },
});

hstring SDKTemplate::StringJoin(hstring const& delimiter, IVectorView<hstring> const& vector)
{
    hstring result;

    bool first = true;
    for (hstring value : vector)
    {
        if (!first)
        {
            result = result + delimiter;
        }
        result = result + value;
    }
    return result;
}

hstring winrt::to_hstring(DayOfWeek value)
{
    switch (value)
    {
    case DayOfWeek::Sunday: return L"Sunday";
    case DayOfWeek::Monday: return L"Monday";
    case DayOfWeek::Tuesday: return L"Tuesday";
    case DayOfWeek::Wednesday: return L"Wednesday";
    case DayOfWeek::Thursday: return L"Thursday";
    case DayOfWeek::Friday: return L"Friday";
    case DayOfWeek::Saturday: return L"Saturday";
    }
    return to_hstring(static_cast<int32_t>(value));
}