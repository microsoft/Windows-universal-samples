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
#include "Scenario2_DeviceWatcher.h"
#include "SampleConfiguration.h"
#include "DeviceHelpers.h"
#include "DeviceWatcherHelper.h"
#include "Scenario2_DeviceWatcher.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_DeviceWatcher::Scenario2_DeviceWatcher()
    {
        InitializeComponent();
    }

    void Scenario2_DeviceWatcher::OnNavigatedTo(NavigationEventArgs const&)
    {
        resultsListView().ItemsSource(resultCollection);

        selectorComboBox().ItemsSource(DeviceSelectorChoices::DeviceWatcherSelectors());
        selectorComboBox().SelectedIndex(0);
    }

    void Scenario2_DeviceWatcher::OnNavigatedFrom(NavigationEventArgs const&)
    {
        deviceWatcherHelper->Reset();
    }

    void Scenario2_DeviceWatcher::StartWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StartWatcher();
    }

    void Scenario2_DeviceWatcher::StopWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StopWatcher();
    }

    void Scenario2_DeviceWatcher::StartWatcher()
    {
        startWatcherButton().IsEnabled(false);
        resultCollection.Clear();

        // First get the device selector chosen by the UI.
        IInspectable item = selectorComboBox().SelectedItem();
        DeviceSelectorInfo* deviceSelectorInfo = FromInspectable<DeviceSelectorInfo>(item);

        DeviceWatcher deviceWatcher{ nullptr };
        if (deviceSelectorInfo->Selector().empty())
        {
            // If the a pre-canned device class selector was chosen, call the DeviceClass overload
            deviceWatcher = DeviceInformation::CreateWatcher(deviceSelectorInfo->DeviceClassSelector());
        }
        else if (deviceSelectorInfo->Kind() == DeviceInformationKind::Unknown)
        {
            // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
            // Kind will be determined by the selector
            deviceWatcher = DeviceInformation::CreateWatcher(
                deviceSelectorInfo->Selector(),
                nullptr // don't request additional properties for this sample
            );
        }
        else
        {
            // Kind is specified in the selector info
            deviceWatcher = DeviceInformation::CreateWatcher(
                deviceSelectorInfo->Selector(),
                nullptr, // don't request additional properties for this sample
                deviceSelectorInfo->Kind());
        }

        rootPage.NotifyUser(L"Starting Watcher...", NotifyType::StatusMessage);
        deviceWatcherHelper->StartWatcher(deviceWatcher);
        stopWatcherButton().IsEnabled(true);
    }

    void Scenario2_DeviceWatcher::StopWatcher()
    {
        stopWatcherButton().IsEnabled(false);
        deviceWatcherHelper->StopWatcher();
        startWatcherButton().IsEnabled(true);
    }
}
