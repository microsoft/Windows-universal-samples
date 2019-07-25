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

#include "Scenario2_DeviceWatcher.g.h"
#include "MainPage.h"
#include "DeviceWatcherHelper.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_DeviceWatcher : Scenario2_DeviceWatcherT<Scenario2_DeviceWatcher>
    {
        Scenario2_DeviceWatcher();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void StartWatcherButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void StopWatcherButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::DeviceInformationDisplay> resultCollection = winrt::single_threaded_observable_vector<SDKTemplate::DeviceInformationDisplay>();
        com_ptr<DeviceWatcherHelper> deviceWatcherHelper = make_self<DeviceWatcherHelper>(resultCollection, Dispatcher());

        void StartWatcher();
        void StopWatcher();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_DeviceWatcher : Scenario2_DeviceWatcherT<Scenario2_DeviceWatcher, implementation::Scenario2_DeviceWatcher>
    {
    };
}
