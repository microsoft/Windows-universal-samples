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
using namespace BackgroundTask;

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
    auto iter = BackgroundTaskRegistration::AllTasks->First();
    auto hascur = iter->HasCurrent;
    while (hascur)
    {
        auto cur = iter->Current->Value;

        if (cur->Name == SampleBackgroundTaskWithConditionName)
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
void SampleBackgroundTaskWithCondition::AttachProgressAndCompletedHandlers(IBackgroundTaskRegistration^ task)
{
    auto progress = [this](BackgroundTaskRegistration^ task, BackgroundTaskProgressEventArgs^ args)
    {
        auto progress = "Progress: " + args->Progress + "%";
        BackgroundTaskSample::SampleBackgroundTaskWithConditionProgress = progress;
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
