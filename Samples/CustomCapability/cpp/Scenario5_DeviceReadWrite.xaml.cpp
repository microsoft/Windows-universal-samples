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
#include <sstream>

#include "MainPage.xaml.h"
#include "Scenario5_DeviceReadWrite.xaml.h"
#include "DeviceList.h"
#include "Fx2Driver.h"


using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Devices::Custom;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;

DeviceReadWrite::DeviceReadWrite()
{
    InitializeComponent();
}

void DeviceReadWrite::ReadBlock_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto fx2Device = DeviceList::Current->GetSelectedDevice();

    if (fx2Device == nullptr) 
    {
        rootPage->NotifyUser("Fx2 device not connected or accessible", NotifyType::ErrorMessage);
        return;
    }

    auto button = safe_cast<Button^>(sender);
    button->IsEnabled = false;

    // create a data reader and load data into it from the input stream of the 
    // device.
    auto dataReader = ref new DataReader(fx2Device->InputStream);

    int counter = readCounter++;

    LogMessage(L"Read " + counter.ToString() + L" begin");

    create_task(dataReader->LoadAsync(64))
        .then([this, dataReader, counter, button](unsigned int count)
    {
        // Get the message out of the data reader
        String^ message = dataReader->ReadString(dataReader->UnconsumedBufferLength);

        LogMessage(L"Read " + counter.ToString() + L" end: " + message);

        button->IsEnabled = true;
    });
}

void DeviceReadWrite::WriteBlock_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto fx2Device = DeviceList::Current->GetSelectedDevice();

    if (fx2Device == nullptr) 
    {
        rootPage->NotifyUser("Fx2 device not connected or accessible", NotifyType::ErrorMessage);
        return;
    }

    auto button = safe_cast<Button^>(sender);
    button->IsEnabled = false;

    // Generate a string to write to the device
    auto counter = writeCounter++;

    auto dataWriter = ref new DataWriter();

    auto message = ref new String(L"This is message ") + counter;

    dataWriter->WriteString(message);

    LogMessage(L"Write " + counter.ToString() + L" begin: " + message);

    create_task(
        fx2Device->OutputStream->WriteAsync(dataWriter->DetachBuffer())
    ).then([this, counter, button](unsigned int bytesWritten)
    {
        LogMessage(L"Write " + counter.ToString() + L" end: " + bytesWritten.ToString() + L" bytes written");
        button->IsEnabled = true;
    });
}


void DeviceReadWrite::LogMessage(String^ message)
{
    auto span = ref new Span();
    auto run = ref new Run();

    run->Text = message;

    span->Inlines->Append(run);
    span->Inlines->Append(ref new LineBreak());

    OutputText->Inlines->InsertAt(0, span);
}