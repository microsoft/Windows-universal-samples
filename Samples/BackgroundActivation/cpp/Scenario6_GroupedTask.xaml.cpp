//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

//
// Scenario6_GroupedTask.xaml.cpp
// Implementation of the GroupedBackgroundTask class
//

#include "pch.h"
#include "Scenario6_GroupedTask.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;
using namespace concurrency;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

GroupedBackgroundTask::GroupedBackgroundTask()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void GroupedBackgroundTask::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    group = BackgroundTaskSample::GetTaskGroup(BackgroundTaskGroupId, BackgroundTaskGroupFriendlyName);

    //
    // Attach progress and completed handlers to any existing tasks.
    //
    for (auto pair : group->AllTasks)
    {
        auto task = pair->Value;
        if (task->Name == GroupedBackgroundTaskName)
        {
            BackgroundTaskSample::UpdateBackgroundTaskRegistrationStatus(task->Name, true);
            AttachProgressAndCompletedHandlers(task);
            break;
        }
    }

    UpdateUI();
}

/// <summary>
/// Attach progress and completed handers to a background task.
/// </summary>
/// <param name="task">The task to attach progress and completed handlers to.</param>
void GroupedBackgroundTask::AttachProgressAndCompletedHandlers(IBackgroundTaskRegistration^ task)
{
    task->Progress += ref new BackgroundTaskProgressEventHandler(this, &GroupedBackgroundTask::OnProgress);
    task->Completed += ref new BackgroundTaskCompletedEventHandler(this, &GroupedBackgroundTask::OnCompleted);
}

/// <summary>
/// Register a Grouped Background Task.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void GroupedBackgroundTask::RegisterGroupedBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto task = BackgroundTaskSample::RegisterBackgroundTask(nullptr,
        GroupedBackgroundTaskName,
        ref new SystemTrigger(SystemTriggerType::TimeZoneChange, false),
        nullptr,
        group);
    AttachProgressAndCompletedHandlers(task);
    UpdateUI();
}

/// <summary>
/// Unregister a Grouped Background Task.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void GroupedBackgroundTask::UnregisterGroupedTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    BackgroundTaskSample::UnregisterBackgroundTasks(GroupedBackgroundTaskName, group);
    UpdateUI();
}

/// <summary>
/// Unregister all Background Tasks that are not grouped.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void GroupedBackgroundTask::UnregisterUngroupedTasks(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    for (auto pair : BackgroundTaskRegistration::AllTasks)
    {
        auto task = pair->Value;
        task->Unregister(true);
        BackgroundTaskSample::UpdateBackgroundTaskRegistrationStatus(task->Name, false);
    }
}

/// <summary>
/// Handle background task progress.
/// </summary>
/// <param name="task">The task that is reporting progress.</param>
/// <param name="args">Arguments of the progress report.</param>
void GroupedBackgroundTask::OnProgress(BackgroundTaskRegistration^ task, BackgroundTaskProgressEventArgs^ args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        auto progress = "Progress: " + args->Progress + "%";
        BackgroundTaskSample::GroupedBackgroundTaskProgress = progress;
        UpdateUI();
    }));
}

/// <summary>
/// Handle background task completion.
/// </summary>
/// <param name="task">The task that is reporting completion.</param>
/// <param name="args">Arguments of the completion report.</param>
void GroupedBackgroundTask::OnCompleted(BackgroundTaskRegistration^ task, BackgroundTaskCompletedEventArgs^ args)
{
    UpdateUI();
}

/// <summary>
/// Update the scenario UI.
/// </summary>
void GroupedBackgroundTask::UpdateUI()
{
    auto uiDelegate = [this]()
    {
        RegisterButton->IsEnabled = !BackgroundTaskSample::GroupedBackgroundTaskRegistered;
        UnregisterGroupedButton->IsEnabled = BackgroundTaskSample::GroupedBackgroundTaskRegistered;
        Progress->Text = BackgroundTaskSample::GroupedBackgroundTaskProgress;
        Status->Text = BackgroundTaskSample::GetBackgroundTaskStatus(GroupedBackgroundTaskName);
    };
    auto handler = ref new Windows::UI::Core::DispatchedHandler(uiDelegate, Platform::CallbackContext::Any);

    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, handler);
}
