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

#include "Scenario3_BackgroundDeviceWatcher.g.h"
#include "MainPage.h"
#include "DeviceWatcherHelper.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_BackgroundDeviceWatcher : Scenario3_BackgroundDeviceWatcherT<Scenario3_BackgroundDeviceWatcher>
    {
        Scenario3_BackgroundDeviceWatcher();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void StartWatcherButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void StopWatcherButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
        Windows::ApplicationModel::Background::IBackgroundTaskRegistration backgroundTaskRegistration{ nullptr };
        event_token taskCompletedToken{};

        void StartWatcher();
        void StopWatcher();
        void RegisterBackgroundTask(Windows::ApplicationModel::Background::DeviceWatcherTrigger const& deviceWatcherTrigger);
        void UnregisterBackgroundTask(Windows::ApplicationModel::Background::IBackgroundTaskRegistration const& taskReg);
        fire_and_forget OnTaskCompleted(Windows::ApplicationModel::Background::BackgroundTaskRegistration const& task, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs const& args);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_BackgroundDeviceWatcher : Scenario3_BackgroundDeviceWatcherT<Scenario3_BackgroundDeviceWatcher, implementation::Scenario3_BackgroundDeviceWatcher>
    {
    };
}
