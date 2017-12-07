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
// Scenario4_BackgroundTask.xaml.h
// Declaration of the Scenario4_BackgroundTask class
//

#pragma once
#include "Scenario4_BackgroundTask.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    static Platform::String^ SampleBackgroundTaskName = "PedometerBackgroundTask";
    static Platform::String^ SampleBackgroundTaskEntryPoint = "Tasks.PedometerBackgroundTask";
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4_BackgroundTask sealed
    {
    public:
        Scenario4_BackgroundTask();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        void ScenarioRegisterTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ScenarioUnregisterTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnCompleted(Windows::ApplicationModel::Background::BackgroundTaskRegistration^ task, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs^ args);
        void RegisterBackgroundTask();
        void UpdateUIAsync(Platform::String^ status);

    private:
        SDKTemplate::MainPage^ rootPage;
        bool backgroundTaskRegistered;
        static int stepCountGoalOffset;
        int stepCount;
        int stepGoal;
    };
}
