//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
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

namespace CustomSensors
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
        SDKTemplate::MainPage^ m_RootPage;
        Windows::Devices::Enumeration::DeviceWatcher^ m_Watcher;
        Windows::Devices::Sensors::Custom::CustomSensor^ m_CustomSensor;
        Windows::Foundation::EventRegistrationToken m_VisibilityToken;
        Windows::Foundation::EventRegistrationToken m_ReadingToken;
        uint32 m_DesiredReportInterval;
        Microsoft::WRL::Wrappers::CriticalSection m_CritsecCustomSensor;

        void VisibilityChanged(Platform::Object^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ e);
        void ReadingChanged(Windows::Devices::Sensors::Custom::CustomSensor^ sender, Windows::Devices::Sensors::Custom::CustomSensorReadingChangedEventArgs^ e);
        void ScenarioEnable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ScenarioDisable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnCustomSensorAdded(Windows::Devices::Enumeration::DeviceWatcher^ watcher, Windows::Devices::Enumeration::DeviceInformation^ firstDevice);
    };
}
