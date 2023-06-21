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
#include "Scenario1_DataEvents.h"
#include "Scenario1_DataEvents.g.cpp"

using namespace winrt;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_DataEvents::Scenario1_DataEvents()
    {
        InitializeComponent();
    }

    void Scenario1_DataEvents::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (sensor != nullptr)
        {
            sensor.ReadingChanged(sensorReadingChangedToken);
            sensor = nullptr;
        }
    }

    fire_and_forget Scenario1_DataEvents::ScenarioGetSensor_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Disable the button so the user can't click it again while we are already working.
        GetSensorButton().IsEnabled(false);

        // Clear any previous message.
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        // Try to get the default presence sensor.
        try
        {
            sensor = co_await HumanPresenceSensor::GetDefaultAsync();
        }
        catch (hresult_access_denied const&)
        {
            // The user denied this app presence sensor access.
            sensor = nullptr;
        }
        GetSensorButton().IsEnabled(true);

        if (sensor != nullptr)
        {
            VisualStateManager::GoToState(*this, L"Ready", true);

            // Begin listening for data reports.
            sensorReadingChangedToken = sensor.ReadingChanged({ get_weak(), &Scenario1_DataEvents::ReadingChanged });
        }
        else
        {
            rootPage.NotifyUser(L"No presence sensor found", NotifyType::ErrorMessage);
        }
   
    }

    fire_and_forget Scenario1_DataEvents::ReadingChanged(HumanPresenceSensor const&, HumanPresenceSensorReadingChangedEventArgs e)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        HumanPresenceSensorReading reading = e.Reading();
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
