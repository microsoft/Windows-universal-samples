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

#pragma once

#include "Scenario1_DevicePicker.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_DevicePicker : Scenario1_DevicePickerT<Scenario1_DevicePicker>
    {
        Scenario1_DevicePicker();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void PickSingleDeviceButton_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void ShowDevicePickerButton_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::MainPage::Current() };
        Windows::Devices::Enumeration::DevicePicker devicePicker{ nullptr };
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceInformationDisplay> resultCollection = winrt::single_threaded_observable_vector<SDKTemplate::DeviceInformationDisplay>();

        fire_and_forget ShowDevicePicker(bool pickSingle);
        fire_and_forget OnDevicePickerDismissed(Windows::Devices::Enumeration::DevicePicker const&, IInspectable const&);
        fire_and_forget OnDeviceSelected(Windows::Devices::Enumeration::DevicePicker const&, Windows::Devices::Enumeration::DeviceSelectedEventArgs args);

    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_DevicePicker : Scenario1_DevicePickerT<Scenario1_DevicePicker, implementation::Scenario1_DevicePicker>
    {
    };
}
