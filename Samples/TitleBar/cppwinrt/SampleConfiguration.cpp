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
#include "CustomTitleBar.h"

using namespace winrt;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"TitleBar C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Custom colors", xaml_typename<SDKTemplate::Scenario1_Colors>() },
    Scenario{ L"Custom drawing", xaml_typename<SDKTemplate::Scenario2_Extend>() },
});

bool SampleState::areControlsInTitleBar = false;
CustomTitleBar SampleState::customTitleBar{ nullptr };

void SampleState::AreControlsInTitleBar(bool value)
{
    areControlsInTitleBar = value;
    if (customTitleBar != nullptr)
    {
        customTitleBar.EnableControlsInTitleBar(value);
    }
}

void SampleState::AddCustomTitleBar()
{
    if (customTitleBar == nullptr)
    {
        customTitleBar = make<implementation::CustomTitleBar>();
        customTitleBar.EnableControlsInTitleBar(areControlsInTitleBar);

        // Make the main page's content a child of the title bar,
        // and make the title bar the new page content.
        MainPage rootPage = MainPage::Current();
        UIElement mainContent = rootPage.Content();
        rootPage.Content(nullptr);
        customTitleBar.SetPageContent(mainContent);
        rootPage.Content(customTitleBar);;
    }
}

void SampleState::RemoveCustomTitleBar()
{
    if (customTitleBar != nullptr)
    {
        // Take the title bar's page content and make it
        // the window content.
        MainPage rootPage = MainPage::Current();
        rootPage.Content(customTitleBar.SetPageContent(nullptr));
        customTitleBar = nullptr;
    }
}

