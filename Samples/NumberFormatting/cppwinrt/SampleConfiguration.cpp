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
    return L"Number formatting C++/WinRT sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Percent and Permille Formatting", xaml_typename<SDKTemplate::Scenario1_PercentPermilleFormatting>() },
    Scenario{ L"Decimal Formatting", xaml_typename<SDKTemplate::Scenario2_DecimalFormatting>() },
    Scenario{ L"Currency Formatting", xaml_typename<SDKTemplate::Scenario3_CurrencyFormatting>() },
    Scenario{ L"Number Parsing", xaml_typename<SDKTemplate::Scenario4_NumberParsing>() },
    Scenario{ L"Rounding and Padding", xaml_typename<SDKTemplate::Scenario5_RoundingAndPadding>() },
    Scenario{ L"Numeral System Translation", xaml_typename<SDKTemplate::Scenario6_NumeralSystemTranslation>() },
    Scenario{ L"Formatting/Translation using Unicode Extensions", xaml_typename<SDKTemplate::Scenario7_UsingUnicodeExtensions>() }
    });
