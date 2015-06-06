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
// Scenario3_ChangeEvents.xaml.h
// Declaration of the Scenario3_ChangeEvents class
//

#pragma once

#include "pch.h"
#include "Scenario3_ChangeEvents.g.h"
#include "MainPage.xaml.h"

namespace ActivitySensorCPP
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_ChangeEvents sealed
    {
    public:
        Scenario3_ChangeEvents();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        Windows::Devices::Sensors::ActivitySensor^ activitySensor;
        Windows::Foundation::EventRegistrationToken readingToken;

        void ReadingChanged(Windows::Devices::Sensors::ActivitySensor^ sender, Windows::Devices::Sensors::ActivitySensorReadingChangedEventArgs^ e);
        void ScenarioEnableReadingChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ScenarioDisableReadingChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        Concurrency::task<Windows::Devices::Sensors::ActivitySensor^> GetActivitySensorAsync();
    };
}
