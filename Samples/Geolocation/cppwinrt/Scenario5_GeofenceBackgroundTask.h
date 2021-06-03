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

#include "Scenario5_GeofenceBackgroundTask.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario5_GeofenceBackgroundTask : Scenario5_GeofenceBackgroundTaskT<Scenario5_GeofenceBackgroundTask>
    {
        Scenario5_GeofenceBackgroundTask();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        void RegisterBackgroundTask(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void UnregisterBackgroundTask(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };

        static constexpr wchar_t BackgroundTaskName[] = L"SampleGeofenceBackgroundTask";
        static constexpr std::wstring_view BackgroundTaskEntryPoint = name_of<BackgroundTask::GeofenceBackgroundTask>();

        Windows::ApplicationModel::Background::IBackgroundTaskRegistration m_geofenceTask{ nullptr };
        event_token m_taskCompletedToken{};

        fire_and_forget OnCompleted(Windows::ApplicationModel::Background::IBackgroundTaskRegistration sender, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs e);
        void UpdateButtonStates();
        void FillEventListBoxWithExistingEvents();


    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario5_GeofenceBackgroundTask : Scenario5_GeofenceBackgroundTaskT<Scenario5_GeofenceBackgroundTask, implementation::Scenario5_GeofenceBackgroundTask>
    {
    };
}
