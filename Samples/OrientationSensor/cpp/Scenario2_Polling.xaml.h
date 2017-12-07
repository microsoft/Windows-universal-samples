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
// Scenario2_Polling.xaml.h
// Declaration of the Scenario2_Polling class
//

#pragma once

#include "pch.h"
#include "Scenario2_Polling.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Polling sealed
    {
    public:
        Scenario2_Polling();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        void ScenarioEnable();
        void ScenarioDisable();

    private:
        SDKTemplate::MainPage^ rootPage;
        Windows::UI::Core::CoreDispatcher^ dispatcher;
        Windows::Devices::Sensors::OrientationSensor^ sensor;
        Windows::Foundation::EventRegistrationToken visibilityToken;
        uint32 desiredReportInterval;
        Windows::UI::Xaml::DispatcherTimer^ dispatcherTimer;

        void VisibilityChanged(Platform::Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e);
        void DisplayCurrentReading(Platform::Object^ sender, Platform::Object^ e);
    };
}