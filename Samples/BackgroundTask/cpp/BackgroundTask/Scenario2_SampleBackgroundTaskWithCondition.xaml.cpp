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
// Scenario2_SampleBackgroundTaskWithCondition.xaml.cpp
// Implementation of the SampleBackgroundTaskWithCondition class
//

#include "pch.h"
#include "Scenario2_SampleBackgroundTaskWithCondition.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

using namespace Windows::ApplicationModel::Background;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

SampleBackgroundTaskWithCondition::SampleBackgroundTaskWithCondition()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void SampleBackgroundTaskWithCondition::OnNavigatedTo(NavigationEventArgs^ e)
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
        if (task->Name == SampleBackgroundTaskWithConditionName)
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
void SampleBackgroundTaskWithCondition::AttachProgressAndCompletedHandlers(IBackgroundTaskRegistration^ task)
{
    task->Progress += ref new BackgroundTaskProgressEventHandler(this, &SampleBackgroundTaskWithCondition::OnProgress);
    task->Completed += ref new BackgroundTaskCompletedEventHandler(this, &SampleBackgroundTaskWithCondition::OnCompleted);
}

/// <summary>
/// Register a SampleBackgroundTaskWithCondition.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void SampleBackgroundTaskWithCondition::RegisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto task = BackgroundTaskSample::RegisterBackgroundTask(SampleBackgroundTaskEntryPoint,
                                                             SampleBackgroundTaskWithConditionName,
                                                             ref new SystemTrigger(SystemTriggerType::TimeZoneChange, false),
                                                             ref new SystemCondition(SystemConditionType::InternetAvailable));
    AttachProgressAndCompletedHandlers(task);
    UpdateUI();
}

/// <summary>
/// Unregister a SampleBackgroundTaskWithCondition.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void SampleBackgroundTaskWithCondition::UnregisterBackgroundTask(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    BackgroundTaskSample::UnregisterBackgroundTasks(SampleBackgroundTaskWithConditionName);
    UpdateUI();
}

/// <summary>
/// Handle background task progress.
/// </summary>
/// <param name="task">The task that is reporting progress.</param>
/// <param name="args">Arguments of the progress report.</param>
void SampleBackgroundTaskWithCondition::OnProgress(BackgroundTaskRegistration^ task, BackgroundTaskProgressEventArgs^ args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        auto progress = "Progress: " + args->Progress + "%";
        BackgroundTaskSample::SampleBackgroundTaskWithConditionProgress = progress;
        UpdateUI();
    }));
}

/// <summary>
/// Handle background task completion.
/// </summary>
/// <param name="task">The task that is reporting completion.</param>
/// <param name="args">Arguments of the completion report.</param>
void SampleBackgroundTaskWithCondition::OnCompleted(BackgroundTaskRegistration^ task, BackgroundTaskCompletedEventArgs^ args)
{
    UpdateUI();
}

/// <summary>
/// Update the scenario UI.
/// </summary>
void SampleBackgroundTaskWithCondition::UpdateUI()
{
    auto uiDelegate = [this]()
    {
        RegisterButton->IsEnabled = !BackgroundTaskSample::SampleBackgroundTaskWithConditionRegistered;
        UnregisterButton->IsEnabled = BackgroundTaskSample::SampleBackgroundTaskWithConditionRegistered;
        Progress->Text = BackgroundTaskSample::SampleBackgroundTaskWithConditionProgress;
        Status->Text = BackgroundTaskSample::GetBackgroundTaskStatus(SampleBackgroundTaskWithConditionName);
    };
    auto handler = ref new Windows::UI::Core::DispatchedHandler(uiDelegate, Platform::CallbackContext::Any);

    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, handler);
}
