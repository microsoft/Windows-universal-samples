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

#include "Scenario1_Events.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// 'Events' scenario page that gets navigated to when the user selected the 'Events' scenario.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Events sealed
    {
    public:
        Scenario1_Events();
    protected:
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnAccessChanged(Windows::Devices::Enumeration::DeviceAccessInformation ^sender, Windows::Devices::Enumeration::DeviceAccessChangedEventArgs ^args);
        void OnReadingChanged(Windows::Devices::Sensors::Pedometer ^sender, Windows::Devices::Sensors::PedometerReadingChangedEventArgs ^args);
        void UpdateEventRegistration(bool registerForEvents);

        SDKTemplate::MainPage^ rootPage;
        Windows::Devices::Enumeration::DeviceAccessInformation^ deviceAccessInfo;
        Windows::Devices::Sensors::Pedometer^ pedometer;
        Windows::Foundation::EventRegistrationToken readingToken;
        bool registeredForEvents;

        int32 totalCumulativeSteps;
        int32 unknownStepCount;
        int32 walkingStepCount;
        int32 runningStepCount;
    };
}
