//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "MainPage.xaml.h"
#include "BackgroundActivity.h"

using namespace SDKTemplate;
using namespace Windows::ApplicationModel::Background;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::System::Threading;

void BackgroundActivity::Run(IBackgroundTaskInstance^ taskInstance)
{
    //
    // Query BackgroundWorkCost
    // Guidance: If BackgroundWorkCost is high, then perform only the minimum amount
    // of work in the background task and return immediately.
    //
    auto cost = BackgroundWorkCost::CurrentBackgroundWorkCost;

    //
    // Associate a cancellation handler with the background task.
    //
    taskInstance->Canceled += ref new BackgroundTaskCanceledEventHandler(this, &BackgroundActivity::OnCanceled);

    //
    // Get the deferral object from the task instance, and take a reference to the taskInstance.
    //
    TaskDeferral = taskInstance->GetDeferral();
    TaskInstance = taskInstance;

    auto timerDelegate = [this](ThreadPoolTimer^ timer)
    {
        if ((CancelRequested == false) &&
            (Progress < 100))
        {
            Progress += 10;
            TaskInstance->Progress = Progress;
        }
        else
        {
            PeriodicTimer->Cancel();

            //
            // Record that this background task ran.
            //
            String^ taskStatus = (Progress < 100) ? "Canceled with reason: " + CancelReason.ToString() : "Completed";
            auto key = TaskInstance->Task->Name;
            BackgroundTaskSample::TaskStatuses->Insert(key, taskStatus);

            //
            // Indicate that the background task has completed.
            //
            TaskDeferral->Complete();
        }
    };

    TimeSpan period;
    period.Duration = 1000 * 10000; // 1 second
    PeriodicTimer = ThreadPoolTimer::CreatePeriodicTimer(ref new TimerElapsedHandler(timerDelegate), period);
}

//
// Handles background task cancellation.
//
void BackgroundActivity::OnCanceled(IBackgroundTaskInstance^ taskInstance, BackgroundTaskCancellationReason reason)
{
    //
    // Indicate that the background task is canceled.
    //
    CancelRequested = true;
    CancelReason = reason;
}

void BackgroundActivity::Start(IBackgroundTaskInstance^ taskInstance)
{
    // Use the taskInstance->Name and/or taskInstance->InstanceId to determine
    // what background activity to perform. In this sample, all of our
    // background activities are the same, so there is nothing to check.
    auto activity = ref new BackgroundActivity();
    activity->Run(taskInstance);
}