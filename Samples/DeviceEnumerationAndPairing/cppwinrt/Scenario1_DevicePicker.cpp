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
#include "Scenario1_DevicePicker.h"
#include "SampleConfiguration.h"
#include "DeviceHelpers.h"
#include "Scenario1_DevicePicker.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_DevicePicker::Scenario1_DevicePicker()
    {
        InitializeComponent();
    }

    void Scenario1_DevicePicker::OnNavigatedTo(NavigationEventArgs const&)
    {
        resultsListView().ItemsSource(resultCollection);

        selectorComboBox().ItemsSource(DeviceSelectorChoices::DevicePickerSelectors());
        selectorComboBox().SelectedIndex(0);
    }

    void Scenario1_DevicePicker::OnNavigatedFrom(NavigationEventArgs const&)
    {
        devicePicker = nullptr;
    }

    void Scenario1_DevicePicker::PickSingleDeviceButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ShowDevicePicker(
            true // pickSingle
        );
    }

    void Scenario1_DevicePicker::ShowDevicePickerButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ShowDevicePicker(
            false // pickSingle
        );
    }

    fire_and_forget Scenario1_DevicePicker::ShowDevicePicker(bool pickSingle)
    {
        auto lifetime = get_strong();

        showDevicePickerButton().IsEnabled(false);
        resultCollection.Clear();

        devicePicker = DevicePicker();

        // First get the device selector chosen by the UI.
        IInspectable item = selectorComboBox().SelectedItem();
        DeviceSelectorInfo* deviceSelectorInfo = FromInspectable<DeviceSelectorInfo>(item);

        if (deviceSelectorInfo->Selector().empty())
        {
            // If the a pre-canned device class selector was chosen, call the DeviceClass overload
            devicePicker.Filter().SupportedDeviceClasses().Append(deviceSelectorInfo->DeviceClassSelector());
        }
        else
        {
            // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
            devicePicker.Filter().SupportedDeviceSelectors().Append(deviceSelectorInfo->Selector());
        }

        rootPage.NotifyUser(L"Showing Device Picker", NotifyType::StatusMessage);

        // Calculate the position to show the picker (right below the buttons)
        GeneralTransform ge = pickSingleDeviceButton().TransformToVisual(nullptr);
        Point point = ge.TransformPoint({ 0, 0 });
        Rect rect{ point,
                        { static_cast<float>(pickSingleDeviceButton().ActualWidth()),
                          static_cast<float>(pickSingleDeviceButton().ActualHeight()) }
        };

        if (pickSingle)
        {
            DeviceInformation di = co_await devicePicker.PickSingleDeviceAsync(rect);
            if (nullptr != di)
            {
                resultCollection.Append(make<DeviceInformationDisplay>(di));
            }
            showDevicePickerButton().IsEnabled(true);
        }
        else
        {
            devicePicker.DevicePickerDismissed({ get_weak(), &Scenario1_DevicePicker::OnDevicePickerDismissed });
            devicePicker.DeviceSelected({ get_weak(), &Scenario1_DevicePicker::OnDeviceSelected });

            // Show the picker
            devicePicker.Show(rect);
        }
    }

    fire_and_forget Scenario1_DevicePicker::OnDevicePickerDismissed(DevicePicker const&, IInspectable const&)
    {
        auto lifetime = get_strong();

        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        co_await resume_foreground(Dispatcher());

        showDevicePickerButton().IsEnabled(true);
        rootPage.NotifyUser(L"Hiding Device Picker", NotifyType::StatusMessage);
    }

    fire_and_forget Scenario1_DevicePicker::OnDeviceSelected(DevicePicker const&, DeviceSelectedEventArgs args)
    {
        auto lifetime = get_strong();

        // Since we have the collection databound to a UI element, we need to update the collection on the UI thread.
        co_await resume_foreground(Dispatcher());

        resultCollection.Clear();
        resultCollection.Append(make<DeviceInformationDisplay>(args.SelectedDevice()));
    }
}
