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
// Scenario5_DataEventsBatching.xaml.h
// Declaration of the Scenario5_DataEventsBatching class
//

#pragma once

#include "pch.h"
#include "Scenario5_DataEventsBatching.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario5_DataEventsBatching sealed
    {
    public:
        Scenario5_DataEventsBatching();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        void ScenarioEnable();
        void ScenarioDisable();

    private:
        SDKTemplate::MainPage^ rootPage = MainPage::Current;
        Windows::Devices::Sensors::Accelerometer^ accelerometer;
        Windows::Foundation::EventRegistrationToken visibilityToken;
        Windows::Foundation::EventRegistrationToken readingToken;
        uint32 desiredReportInterval = 0;
        uint32 desiredReportLatency = 0;

        void VisibilityChanged(Platform::Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e);
        void ReadingChanged(Windows::Devices::Sensors::Accelerometer^ sender, Windows::Devices::Sensors::AccelerometerReadingChangedEventArgs^ e);
    };
}
