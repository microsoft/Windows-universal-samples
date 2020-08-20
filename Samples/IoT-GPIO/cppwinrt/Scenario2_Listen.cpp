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
#include "Scenario2_Listen.h"
#include "Scenario2_Listen.g.cpp"
#include "SampleConfiguration.h"

using namespace std::literals::chrono_literals;
using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Devices::Gpio;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Listen::Scenario2_Listen()
    {
        InitializeComponent();
    }

    void Scenario2_Listen::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&)
    {
        StopScenario();
    }

    void Scenario2_Listen::StartScenario()
    {
        // Initialize the GPIO objects.
        GpioController gpio = GpioController::GetDefault();

        // Set up our GPIO pin for setting values.
        // If this next line crashes with a NullReferenceException,
        // then the problem is that there is no GPIO controller on the device.
        setPin = gpio.OpenPin(SET_PIN);

        // Establish initial value and configure pin for output.
        setPin.Write(currentValue);
        setPin.SetDriveMode(GpioPinDriveMode::Output);

        // Set up our GPIO pin for listening for value changes.
        listenPin = gpio.OpenPin(LISTEN_PIN);

        // Configure pin for input and add ValueChanged listener.
        listenPin.SetDriveMode(GpioPinDriveMode::Input);
        listenPin.ValueChanged({ get_weak(), &Scenario2_Listen::Pin_ValueChanged });

        // Start toggling the pin value every 500ms.
        timer = DispatcherTimer();
        timer.Interval(500ms);
        timerEventToken = timer.Tick({ get_weak(), &Scenario2_Listen::Timer_Tick });
        timer.Start();
    }

    void Scenario2_Listen::StopScenario()
    {
        // Stop the timer.
        if (timer != nullptr)
        {
            timer.Tick(timerEventToken);
            timer.Stop();
            timer = nullptr;
        }

        // Release the GPIO pins.
        if (setPin != nullptr)
        {
            setPin.Close();
            setPin = nullptr;
        }
        if (listenPin != nullptr)
        {
            listenPin.ValueChanged(valueChangedEventToken);
            listenPin.Close();
            listenPin = nullptr;
        }
    }

    void Scenario2_Listen::StartStopScenario_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (timer != nullptr)
        {
            StopScenario();
            StartStopButton().Content(box_value(L"Start"));
            ScenarioControls().Visibility(Visibility::Collapsed);
        }
        else
        {
            StartScenario();
            StartStopButton().Content(box_value(L"Stop"));
            ScenarioControls().Visibility(Visibility::Visible);
        }
    }

    fire_and_forget Scenario2_Listen::Pin_ValueChanged(GpioPin const& sender, GpioPinValueChangedEventArgs const& e)
    {
        auto lifetime = get_strong();

        // Report the change in pin value.
        co_await resume_foreground(Dispatcher());
        CurrentPinValue().Text(PinValueToString(listenPin.Read()));;
    }

    void Scenario2_Listen::Timer_Tick(IInspectable const&, IInspectable const&)
    {
        // Toggle the existing pin value.
        currentValue = (currentValue == GpioPinValue::High) ? GpioPinValue::Low : GpioPinValue::High;
        setPin.Write(currentValue);
    }
}
