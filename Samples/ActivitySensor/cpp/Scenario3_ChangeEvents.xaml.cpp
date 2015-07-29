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
// Scenario3_ChangeEvents.xaml.cpp
// Implementation of the Scenario3_ChangeEvents class
//

#include "pch.h"
#include "Scenario3_ChangeEvents.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario3_ChangeEvents::Scenario3_ChangeEvents() : rootPage(MainPage::Current)
{
    InitializeComponent();

    // Register for access changed notifications for activity sensors
    m_deviceAccessInfo = DeviceAccessInformation::CreateFromDeviceClassId(GUID_ActivitySensor_ClassId);
    m_deviceAccessInfo->AccessChanged += ref new TypedEventHandler<DeviceAccessInformation ^, DeviceAccessChangedEventArgs ^>(this, &Scenario3_ChangeEvents::AccessChanged);
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3_ChangeEvents::OnNavigatedTo(NavigationEventArgs^ e)
{
    ScenarioEnableReadingChangedButton->IsEnabled = true;
    ScenarioDisableReadingChangedButton->IsEnabled = false;
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario3_ChangeEvents::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // If the navigation is external to the app do not clean up.
    // This can occur on Phone when suspending the app.
    if (e->NavigationMode == NavigationMode::Forward && e->Uri == nullptr)
    {
        return;
    }

    if (ScenarioDisableReadingChangedButton->IsEnabled)
    {
        if (m_activitySensor != nullptr)
        {
            m_activitySensor->ReadingChanged::remove(m_readingToken);
            m_readingToken = {};
        }
    }
}

/// <summary>
/// This is the event handler for ReadingChanged events.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_ChangeEvents::ReadingChanged(ActivitySensor^ sender, ActivitySensorReadingChangedEventArgs^ e)
{
    // We need to dispatch to the UI thread to display the output
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, e]()
    {
        ActivitySensorReading^ reading = e->Reading;

        ScenarioOutput_Activity->Text = reading->Activity.ToString();
        ScenarioOutput_Confidence->Text = reading->Confidence.ToString();

        auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");
        ScenarioOutput_ReadingTimestamp->Text = timestampFormatter->Format(reading->Timestamp);
    }));
}

/// <summary>
/// This is the click handler for the 'Reading Changed On' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_ChangeEvents::ScenarioEnableReadingChanged(Object^ sender, RoutedEventArgs^ e)
{
    // Determine if we can access activity sensors
    if (m_deviceAccessInfo->CurrentStatus == DeviceAccessStatus::Allowed)
    {
        create_task(GetActivitySensorAsync()).then([this](ActivitySensor^ sensor)
        {
            m_activitySensor = sensor;
            if (m_activitySensor != nullptr)
            {
                m_activitySensor->SubscribedActivities->Append(ActivityType::Walking);
                m_activitySensor->SubscribedActivities->Append(ActivityType::Running);
                m_activitySensor->SubscribedActivities->Append(ActivityType::InVehicle);
                m_activitySensor->SubscribedActivities->Append(ActivityType::Biking);
                m_activitySensor->SubscribedActivities->Append(ActivityType::Fidgeting);

                m_readingToken = m_activitySensor->ReadingChanged::add(
                    ref new TypedEventHandler<ActivitySensor^, ActivitySensorReadingChangedEventArgs^>(this,
                        &Scenario3_ChangeEvents::ReadingChanged));

                // Update the buttons in the UI thread
                Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
                {
                    ScenarioEnableReadingChangedButton->IsEnabled = false;
                    ScenarioDisableReadingChangedButton->IsEnabled = true;
                    rootPage->NotifyUser("Subscribed to reading changes", NotifyType::StatusMessage);
                }));
            }
            else
            {
                rootPage->NotifyUser("No activity sensors found", NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("Access denied to activity sensors", NotifyType::ErrorMessage);
    }
}

/// <summary>
/// This is the click handler for the 'Reading Changed Off' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_ChangeEvents::ScenarioDisableReadingChanged(Object^ sender, RoutedEventArgs^ e)
{
    if (m_activitySensor != nullptr)
    {
        m_activitySensor->ReadingChanged::remove(m_readingToken);
        rootPage->NotifyUser("Unsubscribed from reading changes", NotifyType::StatusMessage);
    }
    m_readingToken = {};

    ScenarioEnableReadingChangedButton->IsEnabled = true;
    ScenarioDisableReadingChangedButton->IsEnabled = false;
}

/// <summary>
/// Invoked when the access to pedometers has changed
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_ChangeEvents::AccessChanged(DeviceAccessInformation ^sender, DeviceAccessChangedEventArgs ^args)
{
    if (args->Status != DeviceAccessStatus::Allowed)
    {
        m_activitySensor = nullptr;

        Dispatcher->RunAsync(
            CoreDispatcherPriority::Normal,
            ref new DispatchedHandler(
                [this]()
        {
            ScenarioEnableReadingChangedButton->IsEnabled = true;
            ScenarioDisableReadingChangedButton->IsEnabled = false;

            rootPage->NotifyUser("Access to the activity sensor has been denied", NotifyType::ErrorMessage);
        },
        CallbackContext::Any));
    }
}

/// <summary>
/// A helper function to return the activity sensor asynchronously.
/// </summary>
task<ActivitySensor^> Scenario3_ChangeEvents::GetActivitySensorAsync()
{
    if (nullptr == m_activitySensor)
    {
        return create_task(ActivitySensor::GetDefaultAsync());
    }
    else
    {
        return create_task([this]() -> ActivitySensor^
        {
            return m_activitySensor;
        });
    }
}
