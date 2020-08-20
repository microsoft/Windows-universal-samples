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
#include "Scenario1_ReadData.h"
#include "Scenario1_ReadData.g.cpp"

using namespace std::literals::chrono_literals;
using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Devices::I2c;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_ReadData::Scenario1_ReadData()
    {
        InitializeComponent();
    }

    void Scenario1_ReadData::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        StopScenario();
    }

    IAsyncAction Scenario1_ReadData::StartScenarioAsync()
    {
        auto lifetime = get_strong();

        hstring i2cDeviceSelector = I2cDevice::GetDeviceSelector();
        IVectorView<DeviceInformation> devices = co_await DeviceInformation::FindAllAsync(i2cDeviceSelector);

        // 0x40 was determined by looking at the datasheet for the HTU21D sensor
        I2cConnectionSettings HTU21D_settings(0x40);

        // If this next line crashes with an OutOfBoundsException,
        // then the problem is that no I2C devices were found.
        //
        // If the next line crashes with Access Denied, then the problem is
        // that access to the I2C device (HTU21D) is denied.
        //
        // The call to FromIdAsync will also crash if the settings are invalid.
        //
        // FromIdAsync produces null if there is a sharing violation on the device.
        // This will result in a NullReferenceException in Timer_Tick below.
        htu21dSensor = co_await I2cDevice::FromIdAsync(devices.GetAt(0).Id(), HTU21D_settings);

        // Start the polling timer.
        timer = DispatcherTimer();
        timer.Interval(500ms);
        timerEventToken = timer.Tick({ get_weak(), &Scenario1_ReadData::Timer_Tick });
        timer.Start();
    }

    void Scenario1_ReadData::StopScenario()
    {
        if (timer != nullptr)
        {
            timer.Tick(timerEventToken);
            timer.Stop();
            timer = nullptr;
        }

        // Release the I2C sensor.
        if (htu21dSensor != nullptr)
        {
            htu21dSensor.Close();
            htu21dSensor = nullptr;
        }
    }

    fire_and_forget Scenario1_ReadData::StartStopScenario_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        if (timer != nullptr)
        {
            StopScenario();
            StartStopButton().Content(box_value(L"Start"));
            ScenarioControls().Visibility(Visibility::Collapsed);
        }
        else
        {
            StartStopButton().IsEnabled(false);
            co_await StartScenarioAsync();
            StartStopButton().IsEnabled(true);
            StartStopButton().Content(box_value(L"Stop"));
            ScenarioControls().Visibility(Visibility::Visible);
        }
    }

    void Scenario1_ReadData::Timer_Tick(IInspectable const&, IInspectable const&)
    {
        // Read data from I2C.
        uint8_t command[1];
        uint8_t humidityData[2];
        uint8_t temperatureData[2];

        // Read humidity.
        command[0] = 0xE5;

        // If this next line crashes with a NullReferenceException, then
        // there was a sharing violation on the device. (See StartScenarioAsync above.)
        //
        // If this next line crashes for some other reason, then there was
        // an error accessing the device.
        htu21dSensor.WriteRead(command, humidityData);

        // Read temperature.
        command[0] = 0xE3;
        // If this next line crashes, then there was an error accessing the sensor.
        htu21dSensor.WriteRead(command, temperatureData);

        // Calculate and report the humidity.
        auto rawHumidityReading = (humidityData[0] << 8) | humidityData[1];
        auto humidityRatio = rawHumidityReading / (float)65536;
        double humidity = -6.0 + (125.0 * humidityRatio);
        CurrentHumidity().Text(to_hstring(humidity));

        // Calculate and report the temperature.
        auto rawTempReading = (temperatureData[0] << 8) | temperatureData[1];
        auto tempRatio = rawTempReading / (float)65536;
        double temperature = (-46.85 + (175.72 * tempRatio)) * 9 / 5 + 32;
        CurrentTemp().Text(to_hstring(temperature));
    }
}
