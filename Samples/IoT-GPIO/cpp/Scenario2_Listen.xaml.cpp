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
// Scenario2_Listen.xaml.cpp
//

#include "pch.h"
#include "Scenario2_Listen.xaml.h"

using namespace SDKTemplate;

using namespace Windows::Devices::Gpio;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_Listen::Scenario2_Listen()
{
    InitializeComponent();
}

void Scenario2_Listen::OnNavigatedFrom(NavigationEventArgs ^e)
{
    StopScenario();
}

void Scenario2_Listen::StartScenario()
{
    auto gpio = GpioController::GetDefault();

    // Set up our GPIO pin for setting values.
    // If this next line crashes with a NullReferenceException,
    // then the problem is that there is no GPIO controller on the device.
    setPin = gpio->OpenPin(SET_PIN);

    // Establish initial value and configure pin for output.
    setPin->Write(currentValue);
    setPin->SetDriveMode(GpioPinDriveMode::Output);

    // Set up our GPIO pin for listening for value changes.
    listenPin = gpio->OpenPin(LISTEN_PIN);

    // Configure pin for input and add ValueChanged listener.
    listenPin->SetDriveMode(GpioPinDriveMode::Input);
    valueChangedToken = listenPin->ValueChanged += ref new TypedEventHandler<GpioPin^, GpioPinValueChangedEventArgs^>(this, &Scenario2_Listen::Pin_ValueChanged);

    // Start toggling the pin value every 500ms.
    timer = ref new DispatcherTimer();
    TimeSpan interval;
    interval.Duration = 500 * 1000 * 10;
    timer->Interval = interval;
    timerToken = timer->Tick += ref new EventHandler<Object^>(this, &Scenario2_Listen::Timer_Tick);
    timer->Start();
}

void Scenario2_Listen::StopScenario()
{
    // Stop the timer.
    if (timer != nullptr)
    {
        timer->Tick -= timerToken;
        timer->Stop();
        timer = nullptr;
    }

    // Release the GPIO pins
    delete setPin;
    setPin = nullptr;

    if (listenPin != nullptr)
    {
        listenPin->ValueChanged -= valueChangedToken;
        delete listenPin;
        listenPin = nullptr;
    }
}

void Scenario2_Listen::StartStopScenario()
{
    if (timer != nullptr)
    {
        StopScenario();
        StartStopButton->Content = "Start";
        ScenarioControls->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
    else
    {
        StartScenario();
        StartStopButton->Content = "Stop";
        ScenarioControls->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
}

void Scenario2_Listen::Pin_ValueChanged(GpioPin ^sender, GpioPinValueChangedEventArgs ^e)
{
    // Report the change in pin value.
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal, 
        ref new Windows::UI::Core::DispatchedHandler([this]() 
        {
            CurrentPinValue->Text = listenPin->Read().ToString();
        }));
}

void Scenario2_Listen::Timer_Tick(Object ^sender, Object ^e)
{
    // Toggle the existing pin value.
    currentValue = (currentValue == GpioPinValue::High) ? GpioPinValue::Low : GpioPinValue::High;
    setPin->Write(currentValue);
}
