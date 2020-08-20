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
using namespace winrt::Windows::Devices::Spi;
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

        hstring spiDeviceSelector = SpiDevice::GetDeviceSelector();
        IVectorView<DeviceInformation> devices = co_await DeviceInformation::FindAllAsync(spiDeviceSelector);

        // 0 = Chip select line to use.
        SpiConnectionSettings ADXL345_Settings(0);

        // 5MHz is the rated speed of the ADXL345 accelerometer.
        ADXL345_Settings.ClockFrequency(5000000);

        // The accelerometer expects an idle-high clock polarity.
        // We use Mode3 to set the clock polarity and phase to: CPOL = 1, CPHA = 1.
        ADXL345_Settings.Mode(SpiMode::Mode3);

        // If this next line crashes with an ArgumentOutOfRangeException,
        // then the problem is that no SPI devices were found.
        //
        // If the next line crashes with Access Denied, then the problem is
        // that access to the SPI device (ADXL345) is denied.
        //
        // The call to FromIdAsync will also crash if the settings are invalid.
        //
        // FromIdAsync produces null if there is a sharing violation on the device.
        // This will result in a NullReferenceException a few lines later.
        adxl345Sensor = co_await SpiDevice::FromIdAsync(devices.GetAt(0).Id(), ADXL345_Settings);

        // 
        // Initialize the accelerometer:
        //
        // For this device, we create 2-byte write buffers:
        // The first byte is the register address we want to write to.
        // The second byte is the contents that we want to write to the register. 
        //

        // 0x31 is address of data format register, 0x01 sets range to +- 4Gs.
        static constexpr uint8_t WriteBuf_DataFormat[] = { 0x31, 0x01 };

        // 0x2D is address of power control register, 0x08 puts the accelerometer into measurement mode.
        static constexpr uint8_t WriteBuf_PowerControl[] = { 0x2D, 0x08 };

        // Write the register settings.
        //
        // If this next line crashes with a NullReferenceException, then
        // there was a sharing violation on the device.
        // (See comment earlier in this function.)
        //
        // If this next line crashes for some other reason, then there was
        // an error communicating with the device.

        adxl345Sensor.Write(WriteBuf_DataFormat);
        adxl345Sensor.Write(WriteBuf_PowerControl);

        // Start the polling timer.
        timer = DispatcherTimer();
        timer.Interval(100ms);
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

        // Release the SPI sensor.
        if (adxl345Sensor != nullptr)
        {
            adxl345Sensor.Close();
            adxl345Sensor = nullptr;
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
        static constexpr uint8_t ACCEL_SPI_RW_BIT = 0x80;   // Bit used in SPI transactions to indicate read/write
        static constexpr uint8_t ACCEL_SPI_MB_BIT = 0x40;   // Bit used to indicate multi-byte SPI transactions
        static constexpr uint8_t ACCEL_REG_X = 0x32;        // Address of the X Axis data register
        static constexpr int32_t ACCEL_RES = 1024;          // The ADXL345 has 10 bit resolution giving 1024 unique values
        static constexpr int32_t ACCEL_DYN_RANGE_G = 8;     // The ADXL345 had a total dynamic range of 8G, since we're configuring it to +-4G 
        static constexpr int32_t UNITS_PER_G = ACCEL_RES / ACCEL_DYN_RANGE_G;  // Ratio of raw int values to G units

        uint8_t ReadBuf[6 + 1];                             // Read buffer of size 6 bytes (2 bytes * 3 axes) + 1 byte padding
        uint8_t RegAddrBuf[1 + 6] = {};                     // Register address buffer of size 1 byte + 6 bytes padding

        // Register address we want to read from with read and multi-byte bit set
        RegAddrBuf[0] = ACCEL_REG_X | ACCEL_SPI_RW_BIT | ACCEL_SPI_MB_BIT;

        // If this next line crashes, then there was an error communicating with the device.
        adxl345Sensor.TransferFullDuplex(RegAddrBuf, ReadBuf);

        // In order to get the raw 16-bit data values, we need to concatenate two 8-bit bytes for each axis
        short AccelerationRawX = static_cast<short>((ReadBuf[2] << 8) | ReadBuf[1]);
        short AccelerationRawY = static_cast<short>((ReadBuf[4] << 8) | ReadBuf[3]);
        short AccelerationRawZ = static_cast<short>((ReadBuf[6] << 8) | ReadBuf[5]);

        // Convert raw values to G's and display them.
        X().Text(to_hstring((double)AccelerationRawX / UNITS_PER_G));
        Y().Text(to_hstring((double)AccelerationRawY / UNITS_PER_G));
        Z().Text(to_hstring((double)AccelerationRawZ / UNITS_PER_G));
    }
}

