// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_DeviceUse.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;

Scenario1_DeviceUse::Scenario1_DeviceUse() :
    rootPage(MainPage::Current),
    _deviceUseBackgroundTaskRegistration(nullptr),
    _deviceUseTrigger(nullptr),
    _refreshTimer(nullptr)
{
    InitializeComponent();

    _accelerometer = Accelerometer::GetDefault();
    if (nullptr != _accelerometer)
    {
        // Save trigger so that we may start the background task later.
        // Only one instance of the trigger can exist at a time. Since the trigger
        // does not implement IDisposable, it may still be in memory when a new
        // trigger is created.
        _deviceUseTrigger = ref new DeviceUseTrigger();

        // Setup a timer to periodically refresh results when the app is visible.
        TimeSpan interval;
        interval.Duration = 1 * 10000000; // 1 second (in 100-nanosecond units)

        _refreshTimer = ref new DispatcherTimer();
        _refreshTimer->Interval = interval;
        _refreshTimer->Tick += ref new EventHandler<Object^>(this, &Scenario1_DeviceUse::RefreshTimer_Tick);

        // If the background task is active, start the refresh timer and activate the "Disable" button.
        // The "IsBackgroundTaskActive" state is set by the background task.
        const bool isBackgroundTaskActive =
            ApplicationData::Current->LocalSettings->Values->HasKey("IsBackgroundTaskActive") &&
            (bool)ApplicationData::Current->LocalSettings->Values->Lookup("IsBackgroundTaskActive");

        ScenarioEnableButton->IsEnabled = !isBackgroundTaskActive;
        ScenarioDisableButton->IsEnabled = isBackgroundTaskActive;

        if (isBackgroundTaskActive)
        {
            _refreshTimer->Start();
        }

        // Store a setting for the background task to read
        ApplicationData::Current->LocalSettings->Values->Insert("IsAppVisible",
            dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(true)));

        _visibilityToken = Window::Current->VisibilityChanged::add(
            ref new WindowVisibilityChangedEventHandler(this, &Scenario1_DeviceUse::VisibilityChanged));
    }
    else
    {
        rootPage->NotifyUser("No accelerometer found", NotifyType::StatusMessage);
    }
}

Scenario1_DeviceUse::~Scenario1_DeviceUse()
{
    ApplicationData::Current->LocalSettings->Values->Insert("IsAppVisible",
        dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(false)));

    if (nullptr != _accelerometer)
    {
        if (nullptr != Window::Current)
        {
            Window::Current->VisibilityChanged::remove(_visibilityToken);
        }

        _refreshTimer->Stop();

        // The default behavior here is to let the background task continue to run when
        // this scenario exits. The background task can be canceled by clicking on the "Disable"
        // button the next time the app is navigated to.
        // To cancel the background task on scenario exit, uncomment this code.
        // if (nullptr != _deviceUseBackgroundTaskRegistration)
        // {
        //    _deviceUseBackgroundTaskRegistration->Unregister(true);
        // }

        // Unregister from the background task completion event because this object
        // is being destroyed (notifications will no longer be valid).
        if (nullptr != _deviceUseBackgroundTaskRegistration)
        {
            _deviceUseBackgroundTaskRegistration->Completed::remove(_taskCompletionToken);
        }
    }
}

/// <summary>
/// This is the event handler for VisibilityChanged events.
/// </summary>
/// <param name="sender"></param>
/// <param name="e">
/// Event data that can be examined for the current visibility state.
/// </param>
void Scenario1_DeviceUse::VisibilityChanged(Object^ sender, VisibilityChangedEventArgs^ e)
{
    if (ScenarioDisableButton->IsEnabled)
    {
        ApplicationData::Current->LocalSettings->Values->Insert("IsAppVisible",
            dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(e->Visible)));

        if (e->Visible)
        {
            _refreshTimer->Start();
        }
        else
        {
            _refreshTimer->Stop();
        }
    }
}

/// <summary>
/// This is the click handler for the 'Enable' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_DeviceUse::ScenarioEnable(Object^ sender, RoutedEventArgs^ e)
{
    if (nullptr != _accelerometer)
    {
        // Make sure this app is allowed to run background tasks.
        // RequestAccessAsync must be called on the UI thread.
        create_task(BackgroundExecutionManager::RequestAccessAsync())
            .then([this](BackgroundAccessStatus accessStatus)
        {
            if ((BackgroundAccessStatus::AlwaysAllowed == accessStatus) ||
                (BackgroundAccessStatus::AllowedSubjectToSystemPolicy == accessStatus))
            {
                try
                {
                    create_task(StartSensorBackgroundTaskAsync(_accelerometer->DeviceId))
                        .then([this](task<bool> startedTask)
                    {
                        try
                        {
                            bool started = startedTask.get();
                            if (started)
                            {
                                _refreshTimer->Start();

                                ScenarioEnableButton->IsEnabled = false;
                                ScenarioDisableButton->IsEnabled = true;
                            }
                        }
                        catch (Exception ^ex)
                        {
                            rootPage->NotifyUser("Exception in background task: " + ex->Message,
                                NotifyType::ErrorMessage);
                        }
                    });
                }
                catch (Exception ^ex)
                {
                    rootPage->NotifyUser("Exception in background task: " + ex->Message,
                        NotifyType::ErrorMessage);
                }
            }
            else
            {
                rootPage->NotifyUser("Background tasks may be disabled for this app",
                    NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("No accelerometer found", NotifyType::StatusMessage);
    }
}

/// <summary>
/// This is the click handler for the 'Disable' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_DeviceUse::ScenarioDisable(Object^ sender, RoutedEventArgs^ e)
{
    ScenarioEnableButton->IsEnabled = true;
    ScenarioDisableButton->IsEnabled = false;

    _refreshTimer->Stop();

    // Cancel and unregister the background task.
    if (nullptr != _deviceUseBackgroundTaskRegistration)
    {
        // Cancel and unregister the background task from the current app session.
        _deviceUseBackgroundTaskRegistration->Unregister(true);
        _deviceUseBackgroundTaskRegistration = nullptr;
    }
    else
    {
        // Cancel and unregister the background task from the previous app session.
        FindAndCancelExistingBackgroundTask();
    }

    ApplicationData::Current->LocalSettings->Values->Insert("IsBackgroundTaskActive",
        dynamic_cast<PropertyValue^>(PropertyValue::CreateBoolean(false)));

    rootPage->NotifyUser("Background task was canceled", NotifyType::StatusMessage);
}

/// <summary>
/// Starts the sensor background task.
/// </summary>
/// <param name="deviceId">Device Id for the sensor to be used by the task.</param>
/// <param name="e"></param>
/// <returns>
///   A Concurrency::task with a boolean result indicating if the background task
///   was started successfully.
/// </returns>
task<bool> Scenario1_DeviceUse::StartSensorBackgroundTaskAsync(String^ deviceId)
{
    // Make sure only 1 task is running.
    FindAndCancelExistingBackgroundTask();

    // Register the background task.
    auto backgroundTaskBuilder = ref new BackgroundTaskBuilder();
    backgroundTaskBuilder->Name = SampleConstants::Scenario1_TaskName;
    backgroundTaskBuilder->TaskEntryPoint = SampleConstants::Scenario1_TaskEntryPoint;

    backgroundTaskBuilder->SetTrigger(_deviceUseTrigger);
    _deviceUseBackgroundTaskRegistration = backgroundTaskBuilder->Register();

    // Make sure we're notified when the task completes or if there is an update.
    _taskCompletionToken = _deviceUseBackgroundTaskRegistration->Completed::add(
        ref new BackgroundTaskCompletedEventHandler(
        this, &Scenario1_DeviceUse::OnBackgroundTaskCompleted));

    try
    {
        // Request a DeviceUse task to use the accelerometer.
        return create_task(_deviceUseTrigger->RequestAsync(deviceId)).
            then([this](DeviceTriggerResult deviceTriggerResult)
        {
            bool started = false;

            switch (deviceTriggerResult)
            {
            case DeviceTriggerResult::Allowed:
                rootPage->NotifyUser("Background task started",
                    NotifyType::StatusMessage);
                started = true;
                break;

            case DeviceTriggerResult::LowBattery:
                rootPage->NotifyUser("Insufficient battery to run the background task",
                    NotifyType::ErrorMessage);
                break;

            case DeviceTriggerResult::DeniedBySystem:
                // The system can deny a task request if the system-wide DeviceUse task limit is reached.
                rootPage->NotifyUser("The system has denied the background task request",
                    NotifyType::ErrorMessage);
                break;

            default:
                rootPage->NotifyUser("Could not start the background task: " +
                    deviceTriggerResult.ToString(), NotifyType::ErrorMessage);
                break;
            }

            return started;
        });
    }
    catch (Platform::COMException^ e)
    {
        if (0x8000000E == e->HResult) // HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION)
        {
            rootPage->NotifyUser("A previous background task is still running, please wait for it to exit",
                NotifyType::ErrorMessage);

            FindAndCancelExistingBackgroundTask();

            return task<bool>([]()
            {
                return false;
            });
        }
        else
        {
            throw e; // let other exceptions bubble up.
        }
    }
}

/// <summary>
/// Finds a previously registered background task for this scenario and cancels it (if present)
/// </summary>
void Scenario1_DeviceUse::FindAndCancelExistingBackgroundTask()
{
    auto backgroundTaskIter = BackgroundTaskRegistration::AllTasks->First();
    auto isBackgroundTask = backgroundTaskIter->HasCurrent;

    while (isBackgroundTask)
    {
        auto backgroundTask = backgroundTaskIter->Current->Value;

        if (SampleConstants::Scenario1_TaskName == backgroundTask->Name)
        {
            backgroundTask->Unregister(true);
            break;
        }

        isBackgroundTask = backgroundTaskIter->MoveNext();
    }
}


/// <summary>
/// This is the tick handler for the Refresh timer.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_DeviceUse::RefreshTimer_Tick(Object^ /*sender*/, Object^ /*e*/)
{
    if (ApplicationData::Current->LocalSettings->Values->HasKey("SampleCount"))
    {
        uint64 sampleCount = (uint64)ApplicationData::Current->LocalSettings->Values->Lookup("SampleCount");
        ScenarioOutput_SampleCount->Text = sampleCount.ToString();
    }
    else
    {
        ScenarioOutput_SampleCount->Text = "No data";
    }
}

/// <summary>
/// This is the background task completion handler.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_DeviceUse::OnBackgroundTaskCompleted(BackgroundTaskRegistration^ /*sender*/,
    BackgroundTaskCompletedEventArgs^ e)
{
    // Dispatch to the UI thread to display the output.
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
        [this, e]()
        {
            // An exception may be thrown if an error occurs in the background task.
            try
            {
                e->CheckResult();
                if (ApplicationData::Current->LocalSettings->Values->HasKey("TaskCancelationReason"))
                {
                    String^ cancelationReason = (String^)ApplicationData::Current->LocalSettings->Values->Lookup("TaskCancelationReason");
                    rootPage->NotifyUser("Background task was stopped, reason: " + cancelationReason,
                        NotifyType::StatusMessage);
                }
            }
            catch (Exception^ ex)
            {
                rootPage->NotifyUser("Exception in background task: " + ex->Message,
                    NotifyType::ErrorMessage);
            }
        },
        CallbackContext::Any
    ));

    // Unregister the background task and let the remaining task finish until completion.
    if (nullptr != _deviceUseBackgroundTaskRegistration)
    {
        _deviceUseBackgroundTaskRegistration->Unregister(false);
        _deviceUseBackgroundTaskRegistration = nullptr;
    }
}
