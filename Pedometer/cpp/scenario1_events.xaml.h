// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_Events.g.h"
#include "MainPage.xaml.h"

namespace PedometerCPP
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
        SDKTemplate::MainPage^ rootPage;
        void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        Windows::Devices::Sensors::Pedometer^ pedometer;
        Windows::Foundation::EventRegistrationToken readingToken;
        bool registeredForEvents;
        void OnReadingChanged(Windows::Devices::Sensors::Pedometer ^sender, Windows::Devices::Sensors::PedometerReadingChangedEventArgs ^args);
        int32 totalCumulativeSteps;
        int32 unknownStepCount;
        int32 walkingStepCount;
        int32 runningStepCount;
        void UpdateEventRegistration(bool registerForEvents);
    };
}
