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
#include "Scenario4_Snapshot.h"
#include "SampleConfiguration.h"
#include "DeviceHelpers.h"
#include "Scenario4_Snapshot.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario4_Snapshot::Scenario4_Snapshot()
    {
        InitializeComponent();
    }

    void Scenario4_Snapshot::OnNavigatedTo(NavigationEventArgs const&)
    {
        resultsListView().ItemsSource(resultCollection);

        selectorComboBox().ItemsSource(DeviceSelectorChoices::FindAllAsyncSelectors());
        selectorComboBox().SelectedIndex(0);
    }

    fire_and_forget Scenario4_Snapshot::FindButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        findButton().IsEnabled(false);
        resultCollection.Clear();

        // First get the device selector chosen by the UI.
        IInspectable item = selectorComboBox().SelectedItem();
        DeviceSelectorInfo* deviceSelectorInfo = FromInspectable<DeviceSelectorInfo>(item);

        rootPage.NotifyUser(L"FindAllAsync operation started...", NotifyType::StatusMessage);

        DeviceInformationCollection deviceInfoCollection{ nullptr };

        if (deviceSelectorInfo->Selector().empty())
        {
            // If a pre-canned device class selector was chosen, call the DeviceClass overload
            deviceInfoCollection = co_await DeviceInformation::FindAllAsync(deviceSelectorInfo->DeviceClassSelector());
        }
        else if (deviceSelectorInfo->Kind() == DeviceInformationKind::Unknown)
        {
            // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
            // Kind will be determined by the selector
            deviceInfoCollection = co_await DeviceInformation::FindAllAsync(
                deviceSelectorInfo->Selector(),
                nullptr // don't request additional properties for this sample
            );
        }
        else
        {
            // Kind is specified in the selector info
            deviceInfoCollection = co_await DeviceInformation::FindAllAsync(
                deviceSelectorInfo->Selector(),
                nullptr, // don't request additional properties for this sample
                deviceSelectorInfo->Kind());
        }

        rootPage.NotifyUser(
            L"FindAllAsync operation completed. " + to_hstring(deviceInfoCollection.Size()) + L" devices found.",
            NotifyType::StatusMessage);

        for (DeviceInformation const& deviceInfo : deviceInfoCollection)
        {
            resultCollection.Append(make<DeviceInformationDisplay>(deviceInfo));
        }

        findButton().IsEnabled(true);
    }
}
