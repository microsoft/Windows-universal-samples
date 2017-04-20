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
#include "Scenario4_DeviceEvents.xaml.h"
#include "MainPage.xaml.h"
#include "DeviceList.h"
#include "Fx2Driver.h"

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Devices::Custom;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

DeviceEvents::DeviceEvents()
{
    InitializeComponent();
}

void DeviceEvents::OnNavigatedTo(NavigationEventArgs^ e)
{
    ClearSwitchStateTable();
    UpdateRegisterButton();
}

void DeviceEvents::OnNavigatedFrom(NavigationEventArgs^ e)
{
    if (running)
    {
        cancelSource.cancel();
        running = false;
    }
}

void DeviceEvents::deviceEventsGet_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    CustomDevice^ fx2Device = DeviceList::Current->GetSelectedDevice();

    if (fx2Device == nullptr) 
    {
        rootPage->NotifyUser("Fx2 device not connected or accessible", NotifyType::ErrorMessage);
        return;
    }

    auto button = safe_cast<Button^>(sender);
    button->IsEnabled = false;

    auto outputBuffer = ref new Buffer(1);

    create_task(
        fx2Device->SendIOControlAsync(
            Fx2Driver::ReadSwitches,
            nullptr,
            outputBuffer
    )).then([this, outputBuffer, button](task<unsigned int> result)
    {
        try
        {
            unsigned int bytesRead = result.get();

            if (bytesRead == 0)
            {
                rootPage->NotifyUser("Fx2 device returned 0 byte interrupt message.  Stopping",
                                     NotifyType::ErrorMessage);
            }
            else
            {
                byte* data = GetArrayFromBuffer(outputBuffer);
                Array<bool>^ switchStateArray = CreateSwitchStateArray(data);
                UpdateSwitchStateTable(switchStateArray);
            }
        }
        catch (Exception^ exception)
        {
            rootPage->NotifyUser(exception->ToString(), NotifyType::ErrorMessage);
        }

        button->IsEnabled = true;
    });
}

void DeviceEvents::deviceEventsBegin_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    CustomDevice^ fx2Device = DeviceList::Current->GetSelectedDevice();

    if (fx2Device == nullptr) 
    {
        rootPage->NotifyUser("Fx2 device not connected or accessible", NotifyType::ErrorMessage);
        return;
    }

    if (!running)
    {
        StartInterruptMessageWorker(fx2Device);
    }

    UpdateRegisterButton();
}

void DeviceEvents::deviceEventsCancel_Click(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    CustomDevice^ fx2Device = DeviceList::Current->GetSelectedDevice();

    if (fx2Device == nullptr) 
    {
        rootPage->NotifyUser("Fx2 device not connected or accessible", NotifyType::ErrorMessage);
        return;
    }

    if (running)
    {
        cancelSource.cancel();
    }

    UpdateRegisterButton();
}

void DeviceEvents::StartInterruptMessageWorker(CustomDevice^ fx2Device)
{
    auto switchMessageBuffer = ref new Buffer(1);

    cancelSource = cancellation_token_source();
    running = true;

    InterruptMessageWorker(fx2Device, switchMessageBuffer);
}

void DeviceEvents::InterruptMessageWorker(CustomDevice^ fx2Device, IBuffer^ switchMessageBuffer)
{
    create_task(
        fx2Device->SendIOControlAsync(
            Fx2Driver::GetInterruptMessage,
            nullptr,
            switchMessageBuffer
        ),
        cancelSource.get_token()
    ).then([this, fx2Device, switchMessageBuffer](task<unsigned int> result)
    {
        bool failure;

        try
        {
            unsigned int bytesRead = result.get();

            if (bytesRead == 0)
            {
                rootPage->NotifyUser("Fx2 device returned 0 byte interrupt message.  Stopping",
                                     NotifyType::ErrorMessage);
                failure = true;
            }
            else
            {
                UpdateSwitchStateTable(CreateSwitchStateArray(GetArrayFromBuffer(switchMessageBuffer)));
                failure = false;
            }

        }
        catch (task_canceled)
        {
            rootPage->NotifyUser("Pending GetInterruptMessage IO Control cancelled", NotifyType::StatusMessage);
            failure = true;
        }
        catch (Exception^ exception)
        {
            rootPage->NotifyUser("Error accessing Fx2 device:\n" + exception->Message, NotifyType::ErrorMessage);
            failure = true;
        }

        if (failure)
        {
            ClearSwitchStateTable();
            running = false;
            UpdateRegisterButton();
        }
        else
        {
            InterruptMessageWorker(fx2Device, switchMessageBuffer);
        }
    }
    );
}

void DeviceEvents::UpdateRegisterButton()
{
    deviceEventsBegin->IsEnabled = !running;
    deviceEventsCancel->IsEnabled = running;
}


void DeviceEvents::ClearSwitchStateTable()
{
    deviceEventsSwitches->Inlines->Clear();
    previousSwitchValues = nullptr;
}

void DeviceEvents::UpdateSwitchStateTable(Array<bool>^ switchStateArray)
{
    DeviceList::CreateBooleanTable(
        deviceEventsSwitches->Inlines,
        switchStateArray,
        previousSwitchValues,
        "Switch Number",
        "Switch State",
        "off",
        "on"
    );
    previousSwitchValues = switchStateArray;
}
