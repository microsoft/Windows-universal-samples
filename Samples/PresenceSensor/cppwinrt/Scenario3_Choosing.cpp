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
#include "Scenario3_Choosing.h"
#include "Scenario3_Choosing.g.cpp"

using namespace winrt;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_Choosing::Scenario3_Choosing()
    {
        InitializeComponent();
    }

    void Scenario3_Choosing::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (watcher != nullptr)
        {
            watcher.Stop();
        }
    }

    fire_and_forget Scenario3_Choosing::ScenarioPick_Click(IInspectable const&, RoutedEventArgs const&)
    {
        DevicePicker picker;
        picker.Appearance().Title(L"Select a presence sensor from which to read data");
        picker.Filter().SupportedDeviceSelectors().Append(HumanPresenceSensor::GetDeviceSelector());

        // Calculate the position of the button next to which we will place the picker.
        GeneralTransform transform = ScenarioPickButton().TransformToVisual(nullptr);
        Rect rect = transform.TransformBounds({ 0.0, 0.0, static_cast<float>(ScenarioPickButton().ActualWidth()), static_cast<float>(ScenarioPickButton().ActualHeight()) });

        DeviceInformation deviceInfo = co_await picker.PickSingleDeviceAsync(rect);
        if (deviceInfo != nullptr)
        {
            co_await ReadOneReadingFromSensorAsync(deviceInfo);
        }
    }

    /// <summary>
    /// Invoked when the device watcher finds a presence sensor
    /// </summary>
    /// <param name="sender">The device watcher</param>
    /// <param name="device">Device information for the presence sensor that was found</param>
    fire_and_forget Scenario3_Choosing::OnHumanPresenceSensorAdded(DeviceWatcher const&, DeviceInformation deviceInfo)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        // The app can optionally ignore sensors which do not satisfy desired criteria.
        // This sample does not apply any filter. See the DeviceEnumerationAndPairing sample
        // for more advanced ways of presenting a list of devices.

        ListBoxItem item;
        item.Content(box_value(deviceInfo.Name()));
        item.Tag(deviceInfo);
        SensorsList().Items().Append(item);
    }

    /// <summary>
    /// Invoked when a presence sensor is removed from the system
    /// </summary>
    /// <param name="sender">The device watcher</param>
    /// <param name="device">Device information for the presence sensor that was removed</param>
    fire_and_forget Scenario3_Choosing::OnHumanPresenceSensorRemoved(DeviceWatcher const&, DeviceInformationUpdate update)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        // Look for the sensor that was removed from the system and erase it from the ListBox.
        hstring removedId = update.Id();

        ItemCollection items = SensorsList().Items();
        uint32_t itemCount = items.Size();
        for (uint32_t index = 0; index < itemCount; index++)
        {
            ListBoxItem item = items.GetAt(index).as<ListBoxItem>();
            DeviceInformation itemDeviceInfo = item.Tag().as<DeviceInformation>();
            if (itemDeviceInfo.Id() == removedId)
            {
                items.RemoveAt(index);
                break;
            }
        }
    }

    /// <summary>
    /// Invoked when a presence sensor's properties change
    /// </summary>
    /// <param name="sender">The device watcher</param>
    /// <param name="device">Device information for the presence sensor that changed</param>
    fire_and_forget Scenario3_Choosing::OnHumanPresenceSensorUpdated(DeviceWatcher const&, DeviceInformationUpdate update)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        // Look for the sensor that was updated.
        hstring updatedId = update.Id();
        for (IInspectable itemInspectable : SensorsList().Items())
        {
            ListBoxItem item = itemInspectable.as<ListBoxItem>();
            DeviceInformation itemDeviceInfo = item.Tag().as<DeviceInformation>();
            if (itemDeviceInfo.Id() == updatedId)
            {
                // Update the DeviceInformation for the sensor that updated.
                itemDeviceInfo.Update(update);
                // Refresh the device name, in case it changed.
                item.Content(box_value(itemDeviceInfo.Name()));
                break;
            }
        }
    }

    fire_and_forget Scenario3_Choosing::OnHumanPresenceSensorEnumerationCompleted(DeviceWatcher const&, IInspectable const&)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        rootPage.NotifyUser(L"All sensors found. Monitoring for changes...", NotifyType::StatusMessage);
    }

    void Scenario3_Choosing::ScenarioStart_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SensorsList().Items().Clear();

        watcher = DeviceInformation::CreateWatcher(HumanPresenceSensor::GetDeviceSelector());
        watcher.Added({ get_weak(), &Scenario3_Choosing::OnHumanPresenceSensorAdded });
        watcher.Removed({ get_weak(), &Scenario3_Choosing::OnHumanPresenceSensorRemoved });
        watcher.Updated({ get_weak(), &Scenario3_Choosing::OnHumanPresenceSensorUpdated });
        watcher.EnumerationCompleted({ get_weak(), &Scenario3_Choosing::OnHumanPresenceSensorEnumerationCompleted });
        watcher.Start();

        rootPage.NotifyUser(L"Looking for sensors...", NotifyType::StatusMessage);

        ScenarioStartButton().IsEnabled(false);
        ScenarioStopButton().IsEnabled(true);
    }

    void Scenario3_Choosing::ScenarioStop_Click(IInspectable const&, RoutedEventArgs const&)
    {
        watcher.Stop();
        watcher = nullptr;

        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        ScenarioStartButton().IsEnabled(true);
        ScenarioStopButton().IsEnabled(false);
    }

    fire_and_forget Scenario3_Choosing::ScenarioReadSelected_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        ListBoxItem item = SensorsList().SelectedItem().as<ListBoxItem>();
        if (item == nullptr)
        {
            rootPage.NotifyUser(L"No sensor is selected.", NotifyType::ErrorMessage);
            co_return;
        }

        DeviceInformation selectedInfo = item.Tag().as<DeviceInformation>();
        co_await ReadOneReadingFromSensorAsync(selectedInfo);
    }

    // Obtains a sensor described by the DeviceInformation and reads one reading from it.
    IAsyncAction Scenario3_Choosing::ReadOneReadingFromSensorAsync(DeviceInformation deviceInfo)
    {
        HumanPresenceSensor sensor{ nullptr };
        try
        {
            sensor = co_await HumanPresenceSensor::FromIdAsync(deviceInfo.Id());
        }
        catch (hresult_access_denied const&)
        {
            // The user denied this app presence sensor access.
            sensor = nullptr;
        }

        EngagementStateTextBlock().Text(L"No data");
        DetectionStateTextBlock().Text(L"No data");
        TimestampTextBlock().Text(L"No data");
        DetectionDistanceTextBlock().Text(L"No data");


        if (sensor != nullptr)
        {
            HumanPresenceSensorReading reading{ nullptr };
            try
            {
                reading = sensor.GetCurrentReading();
            }
            catch (hresult_access_denied const&)
            {
                // The user denied this app presence sensor access.
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
        else
        {
            rootPage.NotifyUser(L"Could not access sensor " +deviceInfo.Name(), NotifyType::ErrorMessage);
        }
    }
}
