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
// Scenario4_BackgroundTask.xaml.cpp
// Implementation of the Scenario4_BackgroundTask class
//

#include "pch.h"
#include "Scenario4_BackgroundTask.xaml.h"
#include "MainPage.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Concurrency;
using namespace Windows::ApplicationModel::Background;;
using namespace Windows::Devices::Sensors;
using namespace Windows::UI::Core;
using namespace Windows::Storage;

int Scenario4_BackgroundTask::stepCountGoalOffset = 50;

Scenario4_BackgroundTask::Scenario4_BackgroundTask()
{
    InitializeComponent();
    stepCount = 0;
    stepGoal = 0;
    backgroundTaskRegistered = false;
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario4_BackgroundTask::OnNavigatedTo(NavigationEventArgs^ e)
{
    // find out if our background task is registered by finding for a name 
    // match amongst the active tasks
    for each (auto cur in BackgroundTaskRegistration::AllTasks)
    {
        auto registration = dynamic_cast<BackgroundTaskRegistration^>(cur->Value);
        if (SampleBackgroundTaskName == registration->Name)
        {
            // Found our task! update the internal state to use it for updating UI
            backgroundTaskRegistered = true;
            break;
        }
    }
    UpdateUIAsync(backgroundTaskRegistered ? "Registered" : "Unregistered");
}

/// <summary>
/// This is the click handler for the 'Register Task' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_BackgroundTask::ScenarioRegisterTask(Object^ sender, RoutedEventArgs^ e)
{
    create_task(BackgroundExecutionManager::RequestAccessAsync()).then([this](BackgroundAccessStatus status) {

        if ((BackgroundAccessStatus::AlwaysAllowed == status) ||
            (BackgroundAccessStatus::AllowedSubjectToSystemPolicy == status))
        {
            RegisterBackgroundTask();
        }
        else
        {
            // In this case, we are not proceeding with task registration although 
            // we could still go ahead and register (in which case registration becomes active only after the app is allowed access to run in the background)
            rootPage->NotifyUser("Background tasks may be disabled for this app", NotifyType::ErrorMessage);
        }
    });
}

/// <summary>
/// This is the click handler for the 'Unregister Task' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_BackgroundTask::ScenarioUnregisterTask(Object^ sender, RoutedEventArgs^ e)
{
    // Loop through all background tasks and unregister any with SampleBackgroundTaskName
    for each (auto cur in BackgroundTaskRegistration::AllTasks)
    {
        auto registration = dynamic_cast<BackgroundTaskRegistration^>(cur->Value);
        if (SampleBackgroundTaskName == registration->Name)
        {
            registration->Unregister(true /*cancelTask*/);
            backgroundTaskRegistered = false;
        }
    }

    if (!backgroundTaskRegistered)
    {
        UpdateUIAsync("Unregistered");
    }
}

/// <summary>
/// This is the event handler for background task completion.
/// </summary>
/// <param name="task">The task that is reporting completion.</param>
/// <param name="args">The completion report arguments.</param>
void Scenario4_BackgroundTask::OnCompleted(BackgroundTaskRegistration^ task, BackgroundTaskCompletedEventArgs^ args)
{
    String^ status = "Completed and Unregistered";
    try
    {
        // check the result to find any errors reported by the background task
        args->CheckResult();
    }
    catch (Exception^ e)
    {
        status = e->Message;
    }
    // Pedometer background triggers are unique in a way as the background 
    // events are one-shot - Once the step-goal associated with the original 
    // task registration has been reached, that step count is a thing-of-past 
    // and shall not fire as the stepcount on that pedometer changes.
    // Unregister the background task or update the step goals here.
    task->Unregister(false);
    backgroundTaskRegistered = false;
    UpdateUIAsync(status);
}

/// <summary>
/// Registers a background task to watch the default pedometer for a specific step goal.
/// </summary>
void Scenario4_BackgroundTask::RegisterBackgroundTask()
{
    // get the default pedometer to register a background task for
    task<Pedometer^> getDefaultTask(Pedometer::GetDefaultAsync());
    getDefaultTask.then([this](task<Pedometer^> task)
    {
        try
        {
            auto sensorToListen = task.get();
            if (nullptr != sensorToListen)
            {
                // Build a background task builder with appropriate entry point
                auto builder = ref new BackgroundTaskBuilder();
                builder->Name = SampleBackgroundTaskName;
                builder->TaskEntryPoint = SampleBackgroundTaskEntryPoint;
                auto currentReadings = sensorToListen->GetCurrentReadings();

                // To define the step goal, figure out the current step count
                stepCount = 0;
                for (auto kind = PedometerStepKind::Unknown; kind <= PedometerStepKind::Running; kind++)
                {
                    if (currentReadings->HasKey(kind))
                    {
                        auto reading = currentReadings->Lookup(kind);
                        stepCount += reading->CumulativeSteps;
                    }
                }

                // calculate the new step goal by adding the goal offset 
                stepGoal = (stepCount + Scenario4_BackgroundTask::stepCountGoalOffset);
                // create a Pedometer data threshold for that step goal
                auto threshold = ref new PedometerDataThreshold(sensorToListen, stepGoal);
                // create a sensor background trigger using that threshold
                auto trigger = ref new SensorDataThresholdTrigger(threshold);

                // set the trigger and register
                builder->SetTrigger(trigger);
                auto task = builder->Register();
                task->Completed += ref new BackgroundTaskCompletedEventHandler(this, &Scenario4_BackgroundTask::OnCompleted);

                backgroundTaskRegistered = true;
                UpdateUIAsync("Registered");
            }
            else
            {
                UpdateUIAsync("No Pedometers available");
            }
        }
        catch (AccessDeniedException^)
        {
            UpdateUIAsync("Access denied to the pedometer");
        }
    });
}

/// <summary>
/// Updates the scenario UI.
/// </summary>
/// <param name="status">The status string to display.</param>
void Scenario4_BackgroundTask::UpdateUIAsync(String^ status)
{
    // Update the buttons in the UI thread
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [this, status]()
    {
        ScenarioRegisterTaskButton->IsEnabled = !backgroundTaskRegistered;
        ScenarioUnregisterTaskButton->IsEnabled = backgroundTaskRegistered;
        ScenarioOutput_TaskRegistration->Text = status;

        auto settings = ApplicationData::Current->LocalSettings;
        ScenarioOutput_CurrentCount->Text = stepCount.ToString();
        ScenarioOutput_StepGoal->Text = stepGoal.ToString();

        ScenarioOutput_UnknownCount->Text = "0";
        ScenarioOutput_WalkingCount->Text = "0";
        ScenarioOutput_RunningCount->Text = "0";

        if (settings->Values->HasKey("ReportCount"))
        {
            ScenarioOutput_ReportCount->Text = safe_cast<String^>(settings->Values->Lookup("ReportCount")); // stored as a string
        }
        if (settings->Values->HasKey("TaskStatus"))
        {
            ScenarioOutput_TaskStatus->Text = safe_cast<String^>(settings->Values->Lookup("TaskStatus")); // stored as a string
        }
        if (settings->Values->HasKey("LastTimestamp"))
        {
            ScenarioOutput_LastTimestamp->Text = safe_cast<String^>(settings->Values->Lookup("LastTimestamp")); // stored as a string
            for (auto stepKind = PedometerStepKind::Unknown; stepKind <= PedometerStepKind::Running; stepKind++)
            {
                if (settings->Values->HasKey(stepKind.ToString()))
                {
                    String^ stepCount = safe_cast<String^>(settings->Values->Lookup(stepKind.ToString()));
                    switch (stepKind)
                    {
                    case Windows::Devices::Sensors::PedometerStepKind::Unknown:
                        ScenarioOutput_UnknownCount->Text = stepCount;
                        break;
                    case Windows::Devices::Sensors::PedometerStepKind::Walking:
                        ScenarioOutput_WalkingCount->Text = stepCount;
                        break;
                    case Windows::Devices::Sensors::PedometerStepKind::Running:
                        ScenarioOutput_RunningCount->Text = stepCount;
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    },
            CallbackContext::Any
        )
        );
}
