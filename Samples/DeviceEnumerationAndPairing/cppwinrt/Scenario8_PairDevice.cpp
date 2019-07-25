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
#include "Scenario8_PairDevice.h"
#include "DeviceHelpers.h"
#include "SampleConfiguration.h"
#include "Scenario8_PairDevice.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario8_PairDevice::Scenario8_PairDevice()
    {
        InitializeComponent();
        deviceWatcherHelper->DeviceChanged({ get_weak(), &Scenario8_PairDevice::OnDeviceListChanged });
    }

    void Scenario8_PairDevice::OnNavigatedTo(NavigationEventArgs const&)
    {
        resultsListView().ItemsSource(resultCollection);

        selectorComboBox().ItemsSource(DeviceSelectorChoices::PairingSelectors());
        selectorComboBox().SelectedIndex(0);
    }

    void Scenario8_PairDevice::OnNavigatedFrom(NavigationEventArgs const&)
    {
        deviceWatcherHelper->Reset();
    }

    void Scenario8_PairDevice::StartWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StartWatcher();
    }

    void Scenario8_PairDevice::StopWatcherButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StopWatcher();
    }

    void Scenario8_PairDevice::StartWatcher()
    {
        startWatcherButton().IsEnabled(false);
        resultCollection.Clear();

        // Get the device selector chosen by the UI then add additional constraints for devices that
        // can be paired or are already paired.
        DeviceSelectorInfo* deviceSelectorInfo = FromInspectable<DeviceSelectorInfo>(selectorComboBox().SelectedItem());
        hstring selector = L"(" + deviceSelectorInfo->Selector() + L") AND (System.Devices.Aep.CanPair:=System.StructuredQueryType.Boolean#True OR System.Devices.Aep.IsPaired:=System.StructuredQueryType.Boolean#True)";

        DeviceWatcher deviceWatcher{ nullptr };
        if (deviceSelectorInfo->Kind() == DeviceInformationKind::Unknown)
        {
            // Kind will be determined by the selector
            deviceWatcher = DeviceInformation::CreateWatcher(
                selector,
                nullptr // don't request additional properties for this sample
            );
        }
        else
        {
            // Kind is specified in the selector info
            deviceWatcher = DeviceInformation::CreateWatcher(
                selector,
                nullptr, // don't request additional properties for this sample
                deviceSelectorInfo->Kind());
        }

        rootPage.NotifyUser(L"Starting Watcher...", NotifyType::StatusMessage);
        deviceWatcherHelper->StartWatcher(deviceWatcher);
        stopWatcherButton().IsEnabled(true);
    }

    void Scenario8_PairDevice::StopWatcher()
    {
        stopWatcherButton().IsEnabled(false);

        deviceWatcherHelper->StopWatcher();

        startWatcherButton().IsEnabled(true);
    }

    void Scenario8_PairDevice::OnDeviceListChanged(DeviceWatcher const&, hstring const& id)
    {
        // If the item being updated is currently "selected", then update the pairing buttons
        auto item = resultsListView().SelectedItem();
        DeviceInformationDisplay* deviceInformationDisplay = FromInspectable<DeviceInformationDisplay>(item);
        if ((deviceInformationDisplay != nullptr) && (deviceInformationDisplay->Id() == id))
        {
            UpdatePairingButtons();
        }
    }

    fire_and_forget Scenario8_PairDevice::PairButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Gray out the pair button and results view while pairing is in progress.
        resultsListView().IsEnabled(false);
        pairButton().IsEnabled(false);
        rootPage.NotifyUser(L"Pairing started. Please wait...", NotifyType::StatusMessage);

        IInspectable item = resultsListView().SelectedItem();
        DeviceInformationDisplay* deviceInfoDisp = FromInspectable<DeviceInformationDisplay>(item);

        DevicePairingResult result = co_await deviceInfoDisp->DeviceInformation().Pairing().PairAsync();

        rootPage.NotifyUser(
            L"Pairing result = " + to_hstring(result.Status()),
            result.Status() == DevicePairingResultStatus::Paired ? NotifyType::StatusMessage : NotifyType::ErrorMessage);

        UpdatePairingButtons();
        resultsListView().IsEnabled(true);
    }

    fire_and_forget Scenario8_PairDevice::UnpairButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Gray out the pair button and results view while unpairing is in progress.
        resultsListView().IsEnabled(false);
        unpairButton().IsEnabled(false);
        rootPage.NotifyUser(L"Unpairing started. Please wait...", NotifyType::StatusMessage);

        IInspectable item = resultsListView().SelectedItem();
        DeviceInformationDisplay* deviceInfoDisp = FromInspectable<DeviceInformationDisplay>(item);

        DeviceUnpairingResult result = co_await deviceInfoDisp->DeviceInformation().Pairing().UnpairAsync();

        rootPage.NotifyUser(
            L"Unpairing result = " + to_hstring(result.Status()),
            result.Status() == DeviceUnpairingResultStatus::Unpaired ? NotifyType::StatusMessage : NotifyType::ErrorMessage);

        UpdatePairingButtons();
        resultsListView().IsEnabled(true);
    }

    void Scenario8_PairDevice::ResultsListView_SelectionChanged(IInspectable const&, SelectionChangedEventArgs const&)
    {
        UpdatePairingButtons();
    }

    void Scenario8_PairDevice::UpdatePairingButtons()
    {
        auto item = resultsListView().SelectedItem();
        DeviceInformationDisplay* deviceInfoDisp = FromInspectable<DeviceInformationDisplay>(item);
        if (deviceInfoDisp != nullptr &&
            deviceInfoDisp->DeviceInformation().Pairing().CanPair() &&
            !deviceInfoDisp->DeviceInformation().Pairing().IsPaired())
        {
            pairButton().IsEnabled(true);
        }
        else
        {
            pairButton().IsEnabled(false);
        }

        if (deviceInfoDisp != nullptr &&
            deviceInfoDisp->DeviceInformation().Pairing().IsPaired())
        {
            unpairButton().IsEnabled(true);
        }
        else
        {
            unpairButton().IsEnabled(false);
        }
    }
}

