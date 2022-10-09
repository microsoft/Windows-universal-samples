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
#include <sstream>
#include "Scenario3_SettingContainer.h"
#include "Scenario3_SettingContainer.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_SettingContainer::Scenario3_SettingContainer()
    {
        InitializeComponent();
    }

    void Scenario3_SettingContainer::CreateContainer_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ApplicationDataContainer container = localSettings.CreateContainer(containerName, ApplicationDataCreateDisposition::Always);

        DisplayOutput();
    }

    void Scenario3_SettingContainer::DeleteContainer_Click(IInspectable const&, RoutedEventArgs const&)
    {
        localSettings.DeleteContainer(containerName);

        DisplayOutput();
    }

    void Scenario3_SettingContainer::WriteSetting_Click(IInspectable const&, RoutedEventArgs const&)
    {
        localSettings.Containers().Lookup(containerName).Values().Insert(settingName, box_value(L"Hello World")); // example value

        DisplayOutput();
    }

    void Scenario3_SettingContainer::DeleteSetting_Click(IInspectable const&, RoutedEventArgs const&)
    {
        localSettings.Containers().Lookup(containerName).Values().Remove(settingName);

        DisplayOutput();
    }

    void Scenario3_SettingContainer::DisplayOutput()
    {
        ApplicationDataContainer container = localSettings.Containers().TryLookup(containerName);
        bool hasSetting = container ? container.Values().HasKey(settingName) : false;

        std::wstringstream formatted;
        formatted << std::boolalpha << L"Container Exists: " << (container != nullptr) << std::endl;
        formatted << L"Setting Exists: " << hasSetting << std::endl;

        OutputTextBlock().Text(formatted.str());

        WriteSetting().IsEnabled(container != nullptr);
        DeleteSetting().IsEnabled(container != nullptr);
    }

    void Scenario3_SettingContainer::OnNavigatedTo(NavigationEventArgs const&)
    {
        DisplayOutput();
    }
}

