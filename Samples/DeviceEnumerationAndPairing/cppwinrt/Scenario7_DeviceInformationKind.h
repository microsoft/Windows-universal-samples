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

#include "Scenario7_DeviceInformationKind.g.h"
#include "MainPage.h"
#include "DeviceWatcherHelper.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario7_DeviceInformationKind : Scenario7_DeviceInformationKindT<Scenario7_DeviceInformationKind>
    {
        Scenario7_DeviceInformationKind();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void StartWatcherButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void StopWatcherButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceInformationDisplay> resultCollection = winrt::single_threaded_observable_vector<SDKTemplate::DeviceInformationDisplay>();
        std::vector<com_ptr<DeviceWatcherHelper>> deviceWatcherHelpers;

        void StartWatchers();
        void StopWatchers(bool reset = false);
        void OnDeviceListChanged(Windows::Devices::Enumeration::DeviceWatcher const& sender, hstring const& id);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario7_DeviceInformationKind : Scenario7_DeviceInformationKindT<Scenario7_DeviceInformationKind, implementation::Scenario7_DeviceInformationKind>
    {
    };
}
