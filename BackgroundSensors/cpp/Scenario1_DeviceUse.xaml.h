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

#include "Scenario1_DeviceUse.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_DeviceUse sealed
    {
    public:
        Scenario1_DeviceUse();
        virtual ~Scenario1_DeviceUse();

    private:
        void VisibilityChanged(Platform::Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e);

        void ScenarioEnable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void ScenarioDisable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        Concurrency::task<bool> StartSensorBackgroundTaskAsync(Platform::String^ deviceId);

        void FindAndCancelExistingBackgroundTask();

        void RefreshTimer_Tick(Platform::Object^ sender, Platform::Object^ e);

        void OnBackgroundTaskCompleted(
            Windows::ApplicationModel::Background::BackgroundTaskRegistration^ sender,
            Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs^ e);

    private:
        MainPage^ rootPage;

        // Used for requesting a DeviceUse trigger task to receive accelerometer data.
        Windows::Devices::Sensors::Accelerometer^ _accelerometer;

        // Used to trigger the DeviceUse background task that consumes the accelerometer.
        Windows::ApplicationModel::Background::DeviceUseTrigger^ _deviceUseTrigger;

        // Used to register the device use background task.
        Windows::ApplicationModel::Background::BackgroundTaskRegistration^ _deviceUseBackgroundTaskRegistration;

        // Used for refreshing the number of samples received when the app is visible.
        Windows::UI::Xaml::DispatcherTimer^ _refreshTimer;

        // Used for unregistering from the VisibilityChanged event.
        Windows::Foundation::EventRegistrationToken _visibilityToken;

        // Used for unregistering from the background task completion event.
        Windows::Foundation::EventRegistrationToken _taskCompletionToken;
    };
}
