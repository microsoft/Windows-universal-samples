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
// Scenario1_SampleBackgroundTask.xaml.cpp
// Implementation of the SampleBackgroundTask class
//

#include "pch.h"
#include "Scenario1_SampleBackgroundTask.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

using namespace Windows::ApplicationModel::Background;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

SampleBackgroundTask::SampleBackgroundTask()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void SampleBackgroundTask::OnNavigatedTo(NavigationEventArgs^ e)
{
    // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
    // as NotifyUser()
    rootPage = MainPage::Current;

    //
    // Attach progress and completed handlers to any existing tasks.
    //
    for (auto pair : BackgroundTaskRegistration::AllTasks)
    {
        auto task = pair->Value;
        if (task->Name == SampleBackgroundTaskName)
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
void SampleBackgroundTask::AttachProgressAndCompletedHandlers(IBackgroundTaskRegistration^ task)
{
    task->Progress += ref new BackgroundTaskProgressEventHandler(this, &SampleBackgroundTask::OnProgress);
    task->Completed += ref new BackgroundTaskCompletedEventHandler(this, &SampleBackgroundTask::OnCompleted);
}

/// <summary>
/// Register a SampleBackgroundTask.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void SampleBackgroundTask::RegisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto task = BackgroundTaskSample::RegisterBackgroundTask(nullptr,
                                                             SampleBackgroundTaskName,
                                                             ref new SystemTrigger(SystemTriggerType::TimeZoneChange, false),
                                                             nullptr);
    AttachProgressAndCompletedHandlers(task);
    UpdateUI();
}

/// <summary>
/// Unregister a SampleBackgroundTask.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void SampleBackgroundTask::UnregisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    BackgroundTaskSample::UnregisterBackgroundTasks(SampleBackgroundTaskName);
    UpdateUI();
}

/// <summary>
/// Handle background task progress.
/// </summary>
/// <param name="task">The task that is reporting progress.</param>
/// <param name="args">Arguments of the progress report.</param>
void SampleBackgroundTask::OnProgress(BackgroundTaskRegistration^ task, BackgroundTaskProgressEventArgs^ args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        auto progress = "Progress: " + args->Progress + "%";
        BackgroundTaskSample::SampleBackgroundTaskProgress = progress;
        UpdateUI();
    }));
}

/// <summary>
/// Handle background task completion.
/// </summary>
/// <param name="task">The task that is reporting completion.</param>
/// <param name="args">Arguments of the completion report.</param>
void SampleBackgroundTask::OnCompleted(BackgroundTaskRegistration^ task, BackgroundTaskCompletedEventArgs^ args)
{
    UpdateUI();
}

/// <summary>
/// Update the scenario UI.
/// </summary>
void SampleBackgroundTask::UpdateUI()
{
    auto uiDelegate = [this]()
    {
        RegisterButton->IsEnabled = !BackgroundTaskSample::SampleBackgroundTaskRegistered;
        UnregisterButton->IsEnabled = BackgroundTaskSample::SampleBackgroundTaskRegistered;
        Progress->Text = BackgroundTaskSample::SampleBackgroundTaskProgress;
        Status->Text = BackgroundTaskSample::GetBackgroundTaskStatus(SampleBackgroundTaskName);
    };
    auto handler = ref new Windows::UI::Core::DispatchedHandler(uiDelegate, Platform::CallbackContext::Any);

    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, handler);
}
