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
// Scenario3_ServicingCompleteTask.xaml.cpp
// Implementation of the ServicingCompleteTask class
//

#include "pch.h"
#include "Scenario3_ServicingCompleteTask.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

using namespace Windows::ApplicationModel::Background;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

ServicingCompleteTask::ServicingCompleteTask()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void ServicingCompleteTask::OnNavigatedTo(NavigationEventArgs^ e)
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
        if (task->Name == ServicingCompleteTaskName)
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
void ServicingCompleteTask::AttachProgressAndCompletedHandlers(IBackgroundTaskRegistration^ task)
{
    task->Progress += ref new BackgroundTaskProgressEventHandler(this, &ServicingCompleteTask::OnProgress);
    task->Completed += ref new BackgroundTaskCompletedEventHandler(this, &ServicingCompleteTask::OnCompleted);
}

/// <summary>
/// Register a ServicingCompleteTask.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void ServicingCompleteTask::RegisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto task = BackgroundTaskSample::RegisterBackgroundTask(nullptr,
                                                             ServicingCompleteTaskName,
                                                             ref new SystemTrigger(SystemTriggerType::ServicingComplete, false),
                                                             nullptr);

    AttachProgressAndCompletedHandlers(task);
    UpdateUI();
}

/// <summary>
/// Unregister a ServicingCompleteTask.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void ServicingCompleteTask::UnregisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    BackgroundTaskSample::UnregisterBackgroundTasks(ServicingCompleteTaskName);
    UpdateUI();
}

/// <summary>
/// Handle background task progress.
/// </summary>
/// <param name="task">The task that is reporting progress.</param>
/// <param name="args">Arguments of the progress report.</param>
void ServicingCompleteTask::OnProgress(BackgroundTaskRegistration^ task, BackgroundTaskProgressEventArgs^ args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        auto progress = "Progress: " + args->Progress + "%";
        BackgroundTaskSample::ServicingCompleteTaskProgress = progress;
        UpdateUI();
    }));
}

/// <summary>
/// Handle background task completion.
/// </summary>
/// <param name="task">The task that is reporting completion.</param>
/// <param name="args">Arguments of the completion report.</param>
void ServicingCompleteTask::OnCompleted(BackgroundTaskRegistration^ task, BackgroundTaskCompletedEventArgs^ args)
{
    UpdateUI();
}

/// <summary>
/// Update the scenario UI.
/// </summary>
void ServicingCompleteTask::UpdateUI()
{
    auto uiDelegate = [this]()
    {
        RegisterButton->IsEnabled = !BackgroundTaskSample::ServicingCompleteTaskRegistered;
        UnregisterButton->IsEnabled = BackgroundTaskSample::ServicingCompleteTaskRegistered;
        Progress->Text = BackgroundTaskSample::ServicingCompleteTaskProgress;
        Status->Text = BackgroundTaskSample::GetBackgroundTaskStatus(ServicingCompleteTaskName);
    };
    auto handler = ref new Windows::UI::Core::DispatchedHandler(uiDelegate, Platform::CallbackContext::Any);

    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, handler);
}
