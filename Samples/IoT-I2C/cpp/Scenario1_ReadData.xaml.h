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
// Scenario1_GetAndSetPin.xaml.h
//

#pragma once

#include "Scenario1_ReadData.g.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_ReadData sealed
    {
    public:
        Scenario1_ReadData();
        void StartStopScenario();

    protected:
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        concurrency::task<void> StartScenarioAsync();
        void StopScenario();
        void Timer_Tick(Platform::Object^ sender, Platform::Object^ e);

    private:
        Windows::UI::Xaml::DispatcherTimer^ timer;
        Windows::Foundation::EventRegistrationToken timerToken;
        Windows::Devices::I2c::I2cDevice^ htu21dSensor;
    };
}
