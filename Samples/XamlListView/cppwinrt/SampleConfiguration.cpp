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
    return L"ListView/GridView C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Simple ListView", xaml_typename<SDKTemplate::Scenario1_SimpleListView>() },
    Scenario{ L"Simple GridView", xaml_typename<SDKTemplate::Scenario2_SimpleGridView>() },
    Scenario{ L"Master Detail and Selection", xaml_typename<SDKTemplate::Scenario3_MasterDetail>() },
    Scenario{ L"Edge Tapped ListView", xaml_typename<SDKTemplate::Scenario4_EdgeTappedListView>() },
    Scenario{ L"Restore Scroll Position", xaml_typename<SDKTemplate::Scenario5_RestoreScrollPosition>() },
    Scenario{ L"Scroll Into View", xaml_typename<SDKTemplate::Scenario6_ScrollIntoView>() },
});
