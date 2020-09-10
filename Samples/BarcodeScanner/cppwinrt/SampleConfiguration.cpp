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
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Barcode Scanner";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"DataReceived event", xaml_typename<SDKTemplate::Scenario1_BasicFunctionality>() },
    Scenario{ L"Release/Retain functionality", xaml_typename<SDKTemplate::Scenario2_MultipleScanners>() },
    Scenario{ L"Active Symbologies", xaml_typename<SDKTemplate::Scenario3_ActiveSymbologies>() },
    Scenario{ L"Symbology Attributes", xaml_typename<SDKTemplate::Scenario4_SymbologyAttributes>() },
    Scenario{ L"Displaying a Barcode Preview", xaml_typename<SDKTemplate::Scenario5_DisplayingBarcodePreview>() },
});
