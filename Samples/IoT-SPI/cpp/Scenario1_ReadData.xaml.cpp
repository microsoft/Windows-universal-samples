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

using namespace SDKTemplate;

using namespace Platform;
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
using namespace Windows::Devices::Spi;
using namespace Windows::Devices::Enumeration;
using namespace Windows::System::Threading;
using namespace concurrency;

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
    String^ spiDeviceSelector = SpiDevice::GetDeviceSelector();
    return create_task(DeviceInformation::FindAllAsync(spiDeviceSelector))
        .then([this](DeviceInformationCollection^ devices)
    {
        // 0 = Chip select line to use.
        auto ADXL345_Settings = ref new SpiConnectionSettings(0);

        // 5MHz is the rated speed of the ADXL345 accelerometer.
        ADXL345_Settings->ClockFrequency = 5000000;

        // The accelerometer expects an idle-high clock polarity.
        // We use Mode3 to set the clock polarity and phase to: CPOL = 1, CPHA = 1.
        ADXL345_Settings->Mode = SpiMode::Mode3;

        // If this next line crashes with an OutOfBoundsException,
        // then the problem is that no SPI devices were found.
        //
        // If the next line crashes with Access Denied, then the problem is
        // that access to the SPI device (ADXL345) is denied.
        //
        // The call to FromIdAsync will also crash if the settings are invalid.
        return create_task(SpiDevice::FromIdAsync(devices->GetAt(0)->Id, ADXL345_Settings));
    }).then([this](SpiDevice^ spiDevice)
    {
        // spiDevice will be null if there is a sharing violation on the device.
        // This will result in a null reference exception a few lines later.
        adxl345Sensor = spiDevice;

        //
        // Initialize the accelerometer:
        //
        // For this device, we create 2-byte write buffers:
        // The first byte is the register address we want to write to.
        // The second byte is the contents that we want to write to the register.
        //
        auto WriteBuf_DataFormat = ref new Array<byte>(2);
        WriteBuf_DataFormat[0] = 0x31; // 0x31 is address of data format register, 0x01 sets range to +- 4Gs
        WriteBuf_DataFormat[1] = 0x01; // 0x31 is address of data format register, 0x01 sets range to +- 4Gs

        auto WriteBuf_PowerControl = ref new Array<byte>(2);
        WriteBuf_PowerControl[0] = 0x2D;    // 0x2D is address of power control register, 0x08 puts the accelerometer into measurement mode
        WriteBuf_PowerControl[1] = 0x08;    // 0x2D is address of power control register, 0x08 puts the accelerometer into measurement mode

        // Write the register settings
        //
        // If this next line crashes with an access violation reading location 0x00000000,
        // then there was a sharing violation on the device.
        // (See comment earlier in this function.)
        //
        // If this next line crashes for some other reason, then there was
        // an error communicating with the device.
        adxl345Sensor->Write(WriteBuf_DataFormat);
        adxl345Sensor->Write(WriteBuf_PowerControl);

        // Start the polling timer.
        timer = ref new DispatcherTimer();
        timer->Interval = TimeSpan{ 100 * 1000 * 10 }; // 100ms
        timerToken = timer->Tick += ref new EventHandler<Object^>(this, &Scenario1_ReadData::Timer_Tick);
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

    // Release the SPI device.
    delete adxl345Sensor;
    adxl345Sensor = nullptr;
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
    const byte ACCEL_SPI_RW_BIT = 0x80; // Bit used in SPI transactions to indicate read/write
    const byte ACCEL_SPI_MB_BIT = 0x40; // Bit used to indicate multi-byte SPI transactions
    const byte ACCEL_REG_X = 0x32;      // Address of the X Axis data register
    const int ACCEL_RES = 1024;         // The ADXL345 has 10 bit resolution giving 1024 unique values
    const int ACCEL_DYN_RANGE_G = 8;    // The ADXL345 had a total dynamic range of 8G, since we're configuring it to +-4G 
    const int UNITS_PER_G = ACCEL_RES / ACCEL_DYN_RANGE_G;  // Ratio of raw int values to G units

    auto ReadBuf = ref new Array<byte>(6 + 1);      // Read buffer of size 6 bytes (2 bytes * 3 axes) + 1 byte padding
    auto RegAddrBuf = ref new Array<byte>(1 + 6);   // Register address buffer of size 1 byte + 6 bytes padding

    // Register address we want to read from with read and multi-byte bit set.
    RegAddrBuf[0] = ACCEL_REG_X | ACCEL_SPI_RW_BIT | ACCEL_SPI_MB_BIT;

    // If this next line crashes, then there was an error communicating with the device.

    adxl345Sensor->TransferFullDuplex(RegAddrBuf, ReadBuf);

    // In order to get the raw 16-bit data values, we need to concatenate two 8-bit bytes for each axis.
    short AccelerationRawX = (ReadBuf[2] << 8) | ReadBuf[1];
    short AccelerationRawY = (ReadBuf[4] << 8) | ReadBuf[3];
    short AccelerationRawZ = (ReadBuf[6] << 8) | ReadBuf[5];

    // Convert raw values to G's and display them.
    X->Text = ((double)AccelerationRawX / UNITS_PER_G).ToString();
    Y->Text = ((double)AccelerationRawY / UNITS_PER_G).ToString();
    Z->Text = ((double)AccelerationRawZ / UNITS_PER_G).ToString();
}
