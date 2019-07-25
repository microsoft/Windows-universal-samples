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

#include "Scenario6_CustomFilterAddedProps.g.h"
#include "MainPage.h"
#include "DeviceWatcherHelper.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario6_CustomFilterAddedProps : Scenario6_CustomFilterAddedPropsT<Scenario6_CustomFilterAddedProps>
    {
        Scenario6_CustomFilterAddedProps();

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
    struct Scenario6_CustomFilterAddedProps : Scenario6_CustomFilterAddedPropsT<Scenario6_CustomFilterAddedProps, implementation::Scenario6_CustomFilterAddedProps>
    {
    };
}
