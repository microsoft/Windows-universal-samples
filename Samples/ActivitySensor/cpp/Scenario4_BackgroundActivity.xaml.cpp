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
// Scenario4_BackgroundActivity.xaml.cpp
// Implementation of the Scenario4_BackgroundActivity class
//

#include "pch.h"
#include "Scenario4_BackgroundActivity.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Sensors;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario4_BackgroundActivity::Scenario4_BackgroundActivity() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario4_BackgroundActivity::OnNavigatedTo(NavigationEventArgs^ e)
{
    for (const auto& cur : BackgroundTaskRegistration::AllTasks)
    {
        auto registration = dynamic_cast<BackgroundTaskRegistration^>(cur->Value);
        if (SampleBackgroundTaskName == registration->Name)
        {
            registered = true;
            break;
        }
    }

    UpdateUIAsync(registered ? "Registered" : "Unregistered");
}

/// <summary>
/// This is the click handler for the 'Register Task' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_BackgroundActivity::ScenarioRegisterTask(Object^ sender, RoutedEventArgs^ e)
{
    // Determine if we can access activity sensors
    auto deviceAccessInfo = DeviceAccessInformation::CreateFromDeviceClassId(GUID_ActivitySensor_ClassId);
    if (deviceAccessInfo->CurrentStatus == DeviceAccessStatus::Allowed)
    {
        // Determine if an activity sensor is present
        // This can also be done using Windows::Devices::Enumeration::DeviceInformation::FindAllAsync
        create_task(ActivitySensor::GetDefaultAsync).then([this](ActivitySensor^ sensor)
        {
            if (nullptr != sensor)
            {
                create_task(BackgroundExecutionManager::RequestAccessAsync()).then([this](BackgroundAccessStatus status)
                {
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
/// This is the click handler for the 'Unregister Task' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_BackgroundActivity::ScenarioUnregisterTask(Object^ sender, RoutedEventArgs^ e)
{
    // Loop through all background tasks and unregister any with SampleBackgroundTaskName
    for (const auto& cur : BackgroundTaskRegistration::AllTasks)
    {
        auto registration = dynamic_cast<BackgroundTaskRegistration^>(cur->Value);
        if (SampleBackgroundTaskName == registration->Name)
        {
            registration->Unregister(true /*cancelTask*/);
            registered = false;
        }
    }

    if (!registered)
    {
        UpdateUIAsync("Unregistered");
    }
}

/// <summary>
/// This is the event handler for background task completion.
/// </summary>
/// <param name="task">The task that is reporting completion.</param>
/// <param name="args">The completion report arguments.</param>
void Scenario4_BackgroundActivity::OnCompleted(BackgroundTaskRegistration^ task, BackgroundTaskCompletedEventArgs^ args)
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
void Scenario4_BackgroundActivity::RegisterBackgroundTask()
{
    auto builder = ref new BackgroundTaskBuilder();
    builder->Name = SampleBackgroundTaskName;
    builder->TaskEntryPoint = SampleBackgroundTaskEntryPoint;

    const unsigned int reportIntervalMs = 3000; // 3 seconds
    auto trigger = ref new ActivitySensorTrigger(reportIntervalMs);

    trigger->SubscribedActivities->Append(ActivityType::Walking);
    trigger->SubscribedActivities->Append(ActivityType::Running);
    trigger->SubscribedActivities->Append(ActivityType::Biking);

    builder->SetTrigger(trigger);

    auto task = builder->Register();
    task->Completed += ref new BackgroundTaskCompletedEventHandler(this, &Scenario4_BackgroundActivity::OnCompleted);

    registered = true;
    UpdateUIAsync("Registered");
}

/// <summary>
/// Updates the scenario UI.
/// </summary>
/// <param name="status">The status string to display.</param>
void Scenario4_BackgroundActivity::UpdateUIAsync(String^ status)
{
    // Update the buttons in the UI thread
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, status]()
    {
        ScenarioRegisterTaskButton->IsEnabled = !registered;
        ScenarioUnregisterTaskButton->IsEnabled = registered;
        ScenarioOutput_TaskRegistration->Text = status;

        auto settings = ApplicationData::Current->LocalSettings;

        if (settings->Values->HasKey("ReportCount"))
        {
            ScenarioOutput_ReportCount->Text = safe_cast<String^>(settings->Values->Lookup("ReportCount")); // stored as a string
        }
        if (settings->Values->HasKey("TaskStatus"))
        {
            ScenarioOutput_TaskStatus->Text = safe_cast<String^>(settings->Values->Lookup("TaskStatus")); // stored as a string
        }
        if (settings->Values->HasKey("LastActivity"))
        {
            ScenarioOutput_LastActivity->Text = safe_cast<String^>(settings->Values->Lookup("LastActivity")); // stored as a string
        }
        if (settings->Values->HasKey("LastConfidence"))
        {
            ScenarioOutput_LastConfidence->Text = safe_cast<String^>(settings->Values->Lookup("LastConfidence")); // stored as a string
        }
        if (settings->Values->HasKey("LastTimestamp"))
        {
            ScenarioOutput_LastTimestamp->Text = safe_cast<String^>(settings->Values->Lookup("LastTimestamp")); // stored as a string
        }
    }));
}
