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
#include "Scenario6_CustomFilterAddedProps.h"
#include "DeviceHelpers.h"
#include "Scenario6_CustomFilterAddedProps.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario6_CustomFilterAddedProps::Scenario6_CustomFilterAddedProps()
    {
        InitializeComponent();
    }

    void Scenario6_CustomFilterAddedProps::OnNavigatedTo(NavigationEventArgs const&)
    {
        resultsListView().ItemsSource(resultCollection);
    }

    void Scenario6_CustomFilterAddedProps::OnNavigatedFrom(NavigationEventArgs const&)
    {
        deviceWatcherHelper->Reset();
    }

    void Scenario6_CustomFilterAddedProps::StartWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StartWatcher();
    }

    void Scenario6_CustomFilterAddedProps::StopWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StopWatcher();
    }

    void Scenario6_CustomFilterAddedProps::StartWatcher()
    {
        aqsFilterTextBox().IsEnabled(false);
        startWatcherButton().IsEnabled(false);
        resultCollection.Clear();

        // Request some additional properties.  In this sample, these extra properties are just used in the ResultsListViewTemplate.
        // In general you just look up the property in the DeviceInformation.Properties.
        // e.g. DeviceInformation.Properties.TryLookup(L"System.Devices.InterfaceClassGuid").
        std::vector<hstring> requestedProperties = {
            L"System.Devices.InterfaceClassGuid",
            L"System.ItemNameDisplay"
        };

        // Use AQS string filter from the text box
        DeviceWatcher deviceWatcher = DeviceInformation::CreateWatcher(
            aqsFilterTextBox().Text(),
            requestedProperties);

        rootPage.NotifyUser(L"Starting Watcher...", NotifyType::StatusMessage);
        deviceWatcherHelper->StartWatcher(deviceWatcher);
        stopWatcherButton().IsEnabled(true);
        stopWatcherButton().Focus(FocusState::Keyboard);
        aqsFilterTextBox().IsEnabled(true);
    }

    void Scenario6_CustomFilterAddedProps::StopWatcher()
    {
        aqsFilterTextBox().IsEnabled(false);
        stopWatcherButton().IsEnabled(false);

        deviceWatcherHelper->StopWatcher();

        startWatcherButton().IsEnabled(true);
        startWatcherButton().Focus(FocusState::Keyboard);
        aqsFilterTextBox().IsEnabled(true);
    }
}
