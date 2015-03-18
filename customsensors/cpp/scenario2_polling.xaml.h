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
// Scenario2_Polling.xaml.h
// Declaration of the Scenario2 class
//

#pragma once

#include "pch.h"
#include "Scenario2_Polling.g.h"
#include "MainPage.xaml.h"

namespace CustomSensors
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

    private:
        SDKTemplate::MainPage^ m_RootPage;
        Windows::UI::Core::CoreDispatcher^ m_Dispatcher;
        Windows::Devices::Enumeration::DeviceWatcher^ m_Watcher;
        Windows::Devices::Sensors::Custom::CustomSensor^ m_CustomSensor;
        Microsoft::WRL::Wrappers::CriticalSection m_CritsecCustomSensor;

        void GetCO2Level(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnCustomSensorAdded(Windows::Devices::Enumeration::DeviceWatcher^ watcher, Windows::Devices::Enumeration::DeviceInformation^ firstDevice);
    };
}
