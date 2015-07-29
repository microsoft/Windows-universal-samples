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
using namespace BackgroundTask;

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
    auto iter = BackgroundTaskRegistration::AllTasks->First();
    auto hascur = iter->HasCurrent;
    while (hascur)
    {
        auto cur = iter->Current->Value;

        if (cur->Name == SampleBackgroundTaskName)
        {
            BackgroundTaskSample::UpdateBackgroundTaskStatus(cur->Name, true);
            AttachProgressAndCompletedHandlers(cur);
            break;
        }

        hascur = iter->MoveNext();
    }

    UpdateUI();
}

/// <summary>
/// Attach progress and completed handers to a background task.
/// </summary>
/// <param name="task">The task to attach progress and completed handlers to.</param>
void SampleBackgroundTask::AttachProgressAndCompletedHandlers(IBackgroundTaskRegistration^ task)
{
    auto progress = [this](BackgroundTaskRegistration^ task, BackgroundTaskProgressEventArgs^ args)
    {
        auto progress = "Progress: " + args->Progress + "%";
        BackgroundTaskSample::SampleBackgroundTaskProgress = progress;
        UpdateUI();
    };
    task->Progress += ref new BackgroundTaskProgressEventHandler(progress);

    auto completed = [this](BackgroundTaskRegistration^ task, BackgroundTaskCompletedEventArgs^ args)
    {
        UpdateUI();
    };
    task->Completed += ref new BackgroundTaskCompletedEventHandler(completed);
}

/// <summary>
/// Register a SampleBackgroundTask.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void SampleBackgroundTask::RegisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto task = BackgroundTaskSample::RegisterBackgroundTask(SampleBackgroundTaskEntryPoint,
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
