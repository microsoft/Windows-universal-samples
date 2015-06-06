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

//
// Scenario4_BackgroundActivity.xaml.h
// Declaration of the Scenario4_BackgroundActivity class
//

#pragma once

#include "pch.h"
#include "Scenario4_BackgroundActivity.g.h"
#include "MainPage.xaml.h"

namespace ActivitySensorCPP
{
    static Platform::String^ SampleBackgroundTaskName = "ActivitySensorBackgroundTask";
    static Platform::String^ SampleBackgroundTaskEntryPoint = "Tasks.ActivitySensorBackgroundTask";

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4_BackgroundActivity sealed
    {
    public:
        Scenario4_BackgroundActivity();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        bool registered;

        void ScenarioRegisterTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ScenarioUnregisterTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnCompleted(Windows::ApplicationModel::Background::BackgroundTaskRegistration^ task, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs^ args);
        void RegisterBackgroundTask();
        void UpdateUIAsync(Platform::String^ status);
    };
}
