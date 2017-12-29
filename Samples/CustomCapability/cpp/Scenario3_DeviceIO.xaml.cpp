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
#include <robuffer.h>
#include "MainPage.xaml.h"
#include "Scenario3_DeviceIO.xaml.h"
#include "DeviceList.h"
#include "Fx2Driver.h"

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Devices::Custom;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

DeviceIO::DeviceIO()
{
    InitializeComponent();
}

void DeviceIO::DeviceIOGet_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    CustomDevice^ fx2Device = DeviceList::Current->GetSelectedDevice();
    if (fx2Device == nullptr)
    {
        rootPage->NotifyUser("Fx2 device not connected or accessible", NotifyType::ErrorMessage);
        return;
    }

    auto outputBuffer = ref new Buffer(1);

    // The I/O control codes and their meanings are defined by the device.
    create_task(fx2Device->SendIOControlAsync(
        Fx2Driver::GetSevenSegmentDisplay,
        nullptr,
        outputBuffer
    )).then([this, outputBuffer](task<unsigned int> result)
    {
        try
        {
            result.get();

            byte* data = GetArrayFromBuffer(outputBuffer);

            rootPage->NotifyUser(
                "The segment display value is " + Fx2Driver::SevenSegmentToDigit(data[0]),
                NotifyType::StatusMessage
            );

        }
        catch (Exception^ exception)
        {
            rootPage->NotifyUser(exception->Message, NotifyType::ErrorMessage);
        }
    });
}


void DeviceIO::DeviceIOSet_Click(Object^ sender, RoutedEventArgs^ e)
{
    CustomDevice^ fx2Device = DeviceList::Current->GetSelectedDevice();
    if (fx2Device == nullptr) 
    {
        rootPage->NotifyUser("Fx2 device not connected or accessible", NotifyType::ErrorMessage);
        return;
    }

    byte val = (byte)(DeviceIOSegmentInput->SelectedIndex + 1);

    auto inputBuffer = ref new Buffer(1);
    byte* data = GetArrayFromBuffer(inputBuffer);
    data[0] = Fx2Driver::DigitToSevenSegment(val);
    inputBuffer->Length = 1;

    // The I/O control codes and their meanings are defined by the device.
    create_task(fx2Device->SendIOControlAsync(
        Fx2Driver::SetSevenSegmentDisplay,
        inputBuffer,
        nullptr
    )).then([this](task<unsigned int> result)
    {
        try
        {
            result.get();
        }
        catch (Exception^ exception)
        {
            rootPage->NotifyUser(exception->Message, NotifyType::ErrorMessage);
            return;
        }
    });
}

