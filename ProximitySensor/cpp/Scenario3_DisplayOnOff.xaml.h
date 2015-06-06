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
// Scenario3_DisplayOnOff.xaml.h
// Declaration of the Scenario3_DisplayOnOff class
//

#pragma once

#include "pch.h"
#include "Scenario3_DisplayOnOff.g.h"
#include "MainPage.xaml.h"

namespace ProximityCPP
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_DisplayOnOff sealed
    {
    public:
        Scenario3_DisplayOnOff();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        Windows::Devices::Enumeration::DeviceWatcher^ watcher;
        Windows::Devices::Sensors::ProximitySensor^ sensor;
        Windows::Devices::Sensors::ProximitySensorDisplayOnOffController^ displayController;

        void OnProximitySensorAdded(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformation^ device);
        void ScenarioEnable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ScenarioDisable(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}