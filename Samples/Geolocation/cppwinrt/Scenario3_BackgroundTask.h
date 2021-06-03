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

#include "Scenario3_BackgroundTask.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_BackgroundTask : Scenario3_BackgroundTaskT<Scenario3_BackgroundTask>
    {
        Scenario3_BackgroundTask();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void RegisterBackgroundTask(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void UnregisterBackgroundTask(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };
        Windows::ApplicationModel::Background::IBackgroundTaskRegistration m_geolocTask;
        event_token m_taskCompletedToken{};

        static constexpr wchar_t BackgroundTaskName[] = L"SampleLocationBackgroundTask";
        static constexpr std::wstring_view BackgroundTaskEntryPoint = name_of<BackgroundTask::LocationBackgroundTask>();

        fire_and_forget OnCompleted(Windows::ApplicationModel::Background::IBackgroundTaskRegistration sender, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs e);
        void UpdateButtonStates();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_BackgroundTask : Scenario3_BackgroundTaskT<Scenario3_BackgroundTask, implementation::Scenario3_BackgroundTask>
    {
    };
}
