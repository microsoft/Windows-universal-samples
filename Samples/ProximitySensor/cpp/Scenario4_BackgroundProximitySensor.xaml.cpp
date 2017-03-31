////*********************************************************
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
// Scenario4_BackgroundProximitySensor.xaml.cpp
// Implementation of the Scenario4_BackgroundProximitySensor class
//

#include "pch.h"
#include "Scenario4_BackgroundProximitySensor.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Storage;
using namespace Concurrency;

Scenario4_BackgroundProximitySensor::Scenario4_BackgroundProximitySensor() : rootPage(MainPage::Current), sensor(nullptr), backgroundTaskRegistered(false)
{
    InitializeComponent();

    watcher = DeviceInformation::CreateWatcher(ProximitySensor::GetDeviceSelector());
    watcher->Added += ref new TypedEventHandler<DeviceWatcher^, DeviceInformation^>(this, &Scenario4_BackgroundProximitySensor::OnProximitySensorAdded);
    watcher->Start();
}

/// <summary>
/// Invoked when the device watcher finds a proximity sensor
/// </summary>
/// <param name="sender">The device watcher</param>
/// <param name="device">Device information for the proximity sensor that was found</param>
void Scenario4_BackgroundProximitySensor::OnProximitySensorAdded(DeviceWatcher^ /*sender*/, DeviceInformation^ device)
{
    if (nullptr == sensor)
    {
        ProximitySensor^ foundSensor = ProximitySensor::FromId(device->Id);
        if (nullptr != foundSensor)
        {
            sensor = foundSensor;
        }
        else
        {
            Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
            {
                rootPage->NotifyUser("Could not get a proximity sensor from the device id", NotifyType::ErrorMessage);
            }, CallbackContext::Any));
        }
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario4_BackgroundProximitySensor::OnNavigatedTo(NavigationEventArgs^ e)
{
    for each (auto cur in BackgroundTaskRegistration::AllTasks)
    {
        auto registration = dynamic_cast<BackgroundTaskRegistration^>(cur->Value);
        if (SampleBackgroundTaskName == registration->Name)
        {
            backgroundTaskRegistered = true;
            break;
        }
    }
    UpdateUIAsync(backgroundTaskRegistered ? "Registered" : "Unregistered");
}

/// <summary>
/// This is the click handler for the 'Enable' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_BackgroundProximitySensor::ScenarioRegisterTask_Click(Object^ /*sender*/, RoutedEventArgs^ /*e*/)
{
    if (nullptr != sensor)
    {
        create_task(BackgroundExecutionManager::RequestAccessAsync()).then([this](BackgroundAccessStatus status) {

            if ((BackgroundAccessStatus::AlwaysAllowed == status) ||
                (BackgroundAccessStatus::AllowedSubjectToSystemPolicy == status))
            {
                RegisterBackgroundTask();
            }
            else
            {
                rootPage->NotifyUser("Background tasks may be disabled for this app", NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("No proximity sensor found", NotifyType::ErrorMessage);
    }
}

/// <summary>
/// This is the click handler for the 'Disable' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_BackgroundProximitySensor::ScenarioUnregisterTask_Click(Object^ sender, RoutedEventArgs^ e)
{
    for each (auto cur in BackgroundTaskRegistration::AllTasks)
    {
        auto registration = dynamic_cast<BackgroundTaskRegistration^>(cur->Value);
        if (SampleBackgroundTaskName == registration->Name)
        {
            registration->Unregister(true);
            backgroundTaskRegistered = false;
            break;
        }
    }
    UpdateUIAsync("Unregistered");
}

/// <summary>
/// This is the event handler for background task completion.
/// </summary>
/// <param name="task">The task that is reporting completion.</param>
/// <param name="args">The completion report arguments.</param>
void Scenario4_BackgroundProximitySensor::OnCompleted(BackgroundTaskRegistration^ task, BackgroundTaskCompletedEventArgs^ args)
{
    String^ status = "Completed";
    try
    {
        args->CheckResult();
    }
    catch (Exception^ e)
    {
        status = e->Message;
    }
    UpdateUIAsync(status);
}

/// <summary>
/// Registers the background task.
/// </summary>
void Scenario4_BackgroundProximitySensor::RegisterBackgroundTask()
{
    // Make sure we have a proximity sensor we want to listen to
    if (nullptr != sensor)
    {
        auto builder = ref new BackgroundTaskBuilder();
        builder->Name = SampleBackgroundTaskName;
        builder->TaskEntryPoint = SampleBackgroundTaskEntryPoint;

        // create a Proximity sensor data threshold, which simply uses the sensor to listen-to
        auto threshold = ref new ProximitySensorDataThreshold(sensor);
        // create a sensor trigger using the data threshold created above
        auto trigger = ref new SensorDataThresholdTrigger(threshold);

        // set the trigger and register
        builder->SetTrigger(trigger);
        auto task = builder->Register();
        task->Completed += ref new BackgroundTaskCompletedEventHandler(this, &Scenario4_BackgroundProximitySensor::OnCompleted);

        backgroundTaskRegistered = true;
        UpdateUIAsync("Registered");
    }
    else
    {
        UpdateUIAsync("No Proximity Sensor is available");
    }
}

/// <summary>
/// Updates the scenario UI.
/// </summary>
/// <param name="status">The status string to display.</param>
void Scenario4_BackgroundProximitySensor::UpdateUIAsync(String^ status)
{
    // Update the status in the UI thread
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [this, status]()
    {
        ScenarioRegisterTaskButton->IsEnabled = !backgroundTaskRegistered;
        ScenarioUnregisterTaskButton->IsEnabled = backgroundTaskRegistered;
        ScenarioOutput_TaskRegistration->Text = status;
        auto settings = ApplicationData::Current->LocalSettings;

        // update the UI based on the data that the background task saved in the application cache
        if (settings->Values->HasKey("TaskStatus"))
        {
            ScenarioOutput_TaskStatus->Text = safe_cast<String^>(settings->Values->Lookup("TaskStatus"));
        }
        if (settings->Values->HasKey("ReportCount"))
        {
            ScenarioOutput_ReportCount->Text = safe_cast<String^>(settings->Values->Lookup("ReportCount"));
        }
        if (settings->Values->HasKey("LastTimestamp"))
        {
            ScenarioOutput_LastTimestamp->Text = safe_cast<String^>(settings->Values->Lookup("LastTimestamp"));
        }
        if (settings->Values->HasKey("Detected"))
        {
            ScenarioOutput_Detected->Text = safe_cast<String^>(settings->Values->Lookup("Detected"));
        }
    },
            CallbackContext::Any
        )
        );
}
