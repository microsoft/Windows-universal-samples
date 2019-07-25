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
#include "Scenario7_DeviceInformationKind.h"
#include "DeviceHelpers.h"
#include "SampleConfiguration.h"
#include "Scenario7_DeviceInformationKind.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario7_DeviceInformationKind::Scenario7_DeviceInformationKind()
    {
        InitializeComponent();
    }

    void Scenario7_DeviceInformationKind::OnNavigatedTo(NavigationEventArgs const&)
    {
        resultsListView().ItemsSource(resultCollection);

        kindComboBox().ItemsSource(DeviceInformationKindChoices::Choices());
        kindComboBox().SelectedIndex(0);
    }

    void Scenario7_DeviceInformationKind::OnNavigatedFrom(NavigationEventArgs const&)
    {
        StopWatchers(/* reset */ true);
    }

    void Scenario7_DeviceInformationKind::StartWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StartWatchers();
    }

    void Scenario7_DeviceInformationKind::StopWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StopWatchers();
    }

    void Scenario7_DeviceInformationKind::StartWatchers()
    {
        startWatcherButton().IsEnabled(false);
        resultCollection.Clear();

        // Create a watcher for each DeviceInformationKind selected by the user
        IInspectable item = kindComboBox().SelectedItem();
        DeviceInformationKindChoice* choice = FromInspectable<DeviceInformationKindChoice>(item);

        for (DeviceInformationKind deviceInfoKind : choice->Kinds())
        {
            DeviceWatcher deviceWatcher = DeviceInformation::CreateWatcher(
                {}, // AQS Filter string
                nullptr, // requested properties
                deviceInfoKind);
            com_ptr<DeviceWatcherHelper> deviceWatcherHelper = make_self<DeviceWatcherHelper>(resultCollection, Dispatcher());
            deviceWatcherHelper->UpdateStatus(false); // we will show our own status messages
            deviceWatcherHelper->DeviceChanged({ get_weak(), &Scenario7_DeviceInformationKind::OnDeviceListChanged });
            deviceWatcherHelpers.push_back(deviceWatcherHelper);

            deviceWatcherHelper->StartWatcher(deviceWatcher);
        }

        stopWatcherButton().IsEnabled(true);
        stopWatcherButton().Focus(FocusState::Keyboard);
    }

    void Scenario7_DeviceInformationKind::StopWatchers(bool reset)
    {
        stopWatcherButton().IsEnabled(false);

        for (com_ptr<DeviceWatcherHelper>& deviceWatcherHelper : deviceWatcherHelpers)
        {
            if (deviceWatcherHelper->IsWatcherRunning())
            {
                deviceWatcherHelper->StopWatcher();
                if (reset)
                {
                    deviceWatcherHelper->Reset();
                }
            }
        }
        deviceWatcherHelpers.clear();

        startWatcherButton().IsEnabled(true);
        startWatcherButton().Focus(FocusState::Keyboard);
    }

    void Scenario7_DeviceInformationKind::OnDeviceListChanged(DeviceWatcher const&, hstring const&)
    {
        int watchersRunning = 0;

        // Count running watchers
        for (com_ptr<DeviceWatcherHelper> const& deviceWatcherHelper : deviceWatcherHelpers)
        {
            if (deviceWatcherHelper->IsWatcherRunning())
            {
                watchersRunning++;
            }
        }

        hstring message = to_hstring(watchersRunning) + L"/" + to_hstring(deviceWatcherHelpers.size()) +
            L" watchers running. " +
            to_hstring(resultCollection.Size()) + L" devices found.";
        rootPage.NotifyUser(message, NotifyType::StatusMessage);
    }
}

