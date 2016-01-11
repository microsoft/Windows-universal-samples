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
// Scenario4_BackgroundProximitySensor.xaml.h
// Declaration of the Scenario4_BackgroundProximitySesnsor class
//

#pragma once

#include "pch.h"
#include "Scenario4_BackgroundProximitySensor.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    static Platform::String^ SampleBackgroundTaskName = "ProximitySensorBackgroundTask";
    static Platform::String^ SampleBackgroundTaskEntryPoint = "Tasks.ProximitySensorBackgroundTask";

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Scenario4_BackgroundProximitySensor sealed
	{
	public:
		Scenario4_BackgroundProximitySensor();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        SDKTemplate::MainPage^ rootPage;
        Windows::Devices::Enumeration::DeviceWatcher^ watcher;
        Windows::Devices::Sensors::ProximitySensor^ sensor;
        bool backgroundTaskRegistered;

        void OnProximitySensorAdded(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformation^ device);
        void ScenarioRegisterTask_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ScenarioUnregisterTask_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnCompleted(Windows::ApplicationModel::Background::BackgroundTaskRegistration^ task, Windows::ApplicationModel::Background::BackgroundTaskCompletedEventArgs^ args);
        void RegisterBackgroundTask();
        void UpdateUIAsync(Platform::String^ status);
	};
}
