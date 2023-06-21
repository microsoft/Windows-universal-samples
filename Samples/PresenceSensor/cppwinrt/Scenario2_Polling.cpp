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
#include "SampleConfiguration.h"
#include "Scenario2_Polling.h"
#include "Scenario2_Polling.g.cpp"

using namespace winrt;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Polling::Scenario2_Polling()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario2_Polling::ScenarioGetSensor_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses DeviceInformation::FindAllAsync to find all
        // HumanPresenceSensor devices. This allows the app to choose a sensor
        // which best fits its requirements.
        //
        // Scenario 1 shows how to get the default HumanPresenceSensor device.

        // Disable the button so the user can't click it again while we are already working.
        GetSensorButton().IsEnabled(false);

        // Clear any previous message.
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        hstring selector = HumanPresenceSensor::GetDeviceSelector();
        DeviceInformationCollection deviceInfos = co_await DeviceInformation::FindAllAsync(selector);
        for (DeviceInformation deviceInfo : deviceInfos)
        {
            HumanPresenceSensor foundSensor{ nullptr };
            try
            {
                foundSensor = co_await HumanPresenceSensor::FromIdAsync(deviceInfo.Id());
            }
            catch (hresult_access_denied const&)
            {
                // The user denied this app presence sensor access.
                foundSensor = nullptr;
            }

            if (foundSensor != nullptr)
            {
                IReference<uint32_t> maxDetectableDistanceInMillimeters = foundSensor.MaxDetectableDistanceInMillimeters();
                IReference<uint32_t> minDetectableDistanceInMillimeters = foundSensor.MinDetectableDistanceInMillimeters();
                if (maxDetectableDistanceInMillimeters != nullptr)
                {
                    // Check if this is the sensor that matches our ranges.
                    // TODO: Customize these values to your application's needs.
                    constexpr uint32_t distanceInMillimetersLValue = 200; // 0.2m
                    constexpr uint32_t distanceInMillimetersRValue = 1200; // 1.2m
                    if (minDetectableDistanceInMillimeters.Value() <= distanceInMillimetersLValue &&
                        maxDetectableDistanceInMillimeters.Value() >= distanceInMillimetersRValue)
                    {
                        rootPage.NotifyUser(L"Found a presence sensor that meets the detection range", NotifyType::StatusMessage);
                        sensor = foundSensor;
                    }
                    else
                    {
                        rootPage.NotifyUser(L"Presence sensor does not meet the detection range, using it anyway", NotifyType::StatusMessage);
                        // We'll use the sensor anyway, to demonstrate how events work.
                        // Your app may decide not to use this sensor and keep looking for another.
                        sensor = foundSensor;
                    }
                }
                else
                {
                    rootPage.NotifyUser(L"Presence sensor does not report detection ranges, using it anyway", NotifyType::StatusMessage);
                    sensor = foundSensor;
                }
            }

            if (sensor != nullptr)
            {
                // We found a sensor we like. We can stop looking now.
                break;
            }
        }

        GetSensorButton().IsEnabled(true);

        if (sensor != nullptr)
        {
            VisualStateManager::GoToState(*this, L"Ready", true);
        }
        else
        {
            rootPage.NotifyUser(L"No presence sensor found", NotifyType::ErrorMessage);
        }
    }

    void Scenario2_Polling::ScenarioGetData_Click(IInspectable const&, RoutedEventArgs const&)
    {
        HumanPresenceSensorReading reading{ nullptr };
        try
        {
            reading = sensor.GetCurrentReading();
        }
        catch (hresult_access_denied const&)
        {
            reading = nullptr;
        }

        if (reading != nullptr)
        {
            if (sensor.IsEngagementSupported())
            {
                EngagementStateTextBlock().Text(to_hstring(reading.Engagement()));
            }
            else
            {
                EngagementStateTextBlock().Text(L"Not Supported");
            }

            if (sensor.IsPresenceSupported())
            {
                DetectionStateTextBlock().Text(to_hstring(reading.Presence()));
            }
            else
            {
                DetectionStateTextBlock().Text(L"Not Supported");
            }

            // Show the detection distance, if available
            IReference<uint32_t> distance = reading.DistanceInMillimeters();
            DetectionDistanceTextBlock().Text(distance == nullptr ? L"Unavailable" : to_hstring(distance.Value()));

            TimestampTextBlock().Text(DateTimeFormatter(L"shortdate longtime").Format(reading.Timestamp()));
        }
    }
}
