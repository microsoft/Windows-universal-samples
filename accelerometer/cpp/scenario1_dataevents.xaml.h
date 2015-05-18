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
// Scenario1_DataEvents.xaml.h
// Declaration of the Scenario1_DataEvents class
//

#pragma once

#include "pch.h"
#include "Scenario1_DataEvents.g.h"
#include "MainPage.xaml.h"

namespace AccelerometerCPP
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_DataEvents sealed
    {
    public:
        Scenario1_DataEvents();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        Windows::Devices::Sensors::Accelerometer^ accelerometer;
        Windows::Foundation::EventRegistrationToken visibilityToken;
        Windows::Foundation::EventRegistrationToken readingToken;
        uint32 desiredReportInterval;

        void VisibilityChanged(Platform::Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e);
        void ReadingChanged(Windows::Devices::Sensors::Accelerometer^ sender, Windows::Devices::Sensors::AccelerometerReadingChangedEventArgs^ e);
        void ScenarioEnable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ScenarioDisable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
