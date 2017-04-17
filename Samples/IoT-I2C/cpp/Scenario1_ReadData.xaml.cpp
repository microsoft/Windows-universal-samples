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

#include "pch.h"
#include "Scenario1_ReadData.xaml.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Devices::I2c;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_ReadData::Scenario1_ReadData()
{
    InitializeComponent();
}

void Scenario1_ReadData::OnNavigatedFrom(NavigationEventArgs^ e)
{
    StopScenario();
}

task<void> Scenario1_ReadData::StartScenarioAsync()
{
    String^ i2cDeviceSelector = I2cDevice::GetDeviceSelector();

    return create_task(DeviceInformation::FindAllAsync(i2cDeviceSelector))
        .then([this](DeviceInformationCollection^ devices) {

        // 0x40 was determined by looking at the datasheet for the HTU21D sensor
        auto HTU21D_settings = ref new I2cConnectionSettings(0x40);

        // If this next line crashes with an OutOfBoundsException,
        // then the problem is that no I2C devices were found.
        //
        // If the next line crashes with Access Denied, then the problem is
        // that access to the I2C device (HTU21D) is denied.
        //
        // The call to FromIdAsync will also crash if the settings are invalid.
        return I2cDevice::FromIdAsync(devices->GetAt(0)->Id, HTU21D_settings);
    }).then([this](I2cDevice^ i2cDevice) {

        // i2cDevice will be nullptr if there is a sharing violation on the device.
        // This will result in an access violation in Timer_Tick below.
        htu21dSensor = i2cDevice;

        // Start the polling timer.
        timer = ref new DispatcherTimer();
        timer->Interval = TimeSpan{ 500 * 1000 * 10 }; // 500ms
        timerToken = timer->Tick += ref new EventHandler<Object^>(this, &SDKTemplate::Scenario1_ReadData::Timer_Tick);
        timer->Start();
    });
}

void Scenario1_ReadData::StopScenario()
{
    if (timer != nullptr)
    {
        timer->Tick -= timerToken;
        timer->Stop();
        timer = nullptr;
    }

    // Release the I2C sensor.
    delete htu21dSensor;
    htu21dSensor = nullptr;
}

void Scenario1_ReadData::StartStopScenario()
{
    if (timer != nullptr)
    {
        StopScenario();
        StartStopButton->Content = "Start";
        ScenarioControls->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
    else
    {
        StartStopButton->IsEnabled = false;
        StartScenarioAsync().then([this]()
        {
            StartStopButton->IsEnabled = true;
            StartStopButton->Content = "Stop";
            ScenarioControls->Visibility = Windows::UI::Xaml::Visibility::Visible;
        });
    }
}

void Scenario1_ReadData::Timer_Tick(Object^ sender, Object^ e)
{
    // Read data from I2C.
    auto command = ref new Array<byte>(1);
    auto humidityData = ref new Array<byte>(2);
    auto temperatureData = ref new Array<byte>(2);


    // Read humidity.
    command[0] = 0xE5;

    // If this next line crashes with an access violation reading location 0x00000000,
    // then there was a sharing violation on the device. (See StartScenarioAsync above.)
    //
    // If this next line crashes for some other reason, then there was
    // an error accessing the sensor.
    htu21dSensor->WriteRead(command, humidityData);

    // Read temperature.
    command[0] = 0xE3;
    // If this next line crashes, then there was an error accessing the sensor.
    htu21dSensor->WriteRead(command, temperatureData);

    // Calculate and report humidity.
    auto rawReadingH = humidityData[0] << 8 | humidityData[1];
    auto ratioH = rawReadingH / (float)65536;
    double humidity = -6 + (125 * ratioH);
    CurrentHumidity->Text = humidity.ToString();

    //Calculate and report temperature.
    auto rawReadingT = temperatureData[0] << 8 | temperatureData[1];
    auto ratioT = rawReadingT / (float)65536;
    double temperature = (-46.85 + (175.72 * ratioT)) * 9 / 5 + 32;
    CurrentTemp->Text = temperature.ToString();
}
