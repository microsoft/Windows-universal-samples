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
// ActivitySensorBackgroundTask.cpp
// Implementation of the ActivitySensorBackgroundTask class
//

#include "pch.h"
#include "ActivitySensorBackgroundTask.h"

using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Storage;

using namespace Tasks;

/// <summary>
/// The entry point of a background task.
/// </summary>
/// <param name="taskInstance">The current background task instance.</param>
void ActivitySensorBackgroundTask::Run(IBackgroundTaskInstance^ taskInstance)
{
    // Associate a cancellation handler with the background task.
    // Even though this task isn't performing much work, it can still be cancelled.
    taskInstance->Canceled += ref new BackgroundTaskCanceledEventHandler(this, &ActivitySensorBackgroundTask::OnCanceled);

    // Read the activity reports
    ActivitySensorTriggerDetails^ triggerDetails = safe_cast<ActivitySensorTriggerDetails^>(taskInstance->TriggerDetails);
    auto reports = triggerDetails->ReadReports();

    auto settings = ApplicationData::Current->LocalSettings;
    settings->Values->Insert("ReportCount", reports->Size.ToString());

    auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");
    if (reports->Size > 0)
    {
        auto lastReading = reports->GetAt(reports->Size - 1)->Reading;
        settings->Values->Insert("LastActivity", lastReading->Activity.ToString());
        settings->Values->Insert("LastConfidence", lastReading->Confidence.ToString());
        settings->Values->Insert("LastTimestamp", timestampFormatter->Format(lastReading->Timestamp));
    }
    else
    {
        settings->Values->Insert("LastActivity", "No data");
        settings->Values->Insert("LastConfidence", "No data");
        settings->Values->Insert("LastTimestamp", "No data");
    }

    auto calendar = ref new Calendar();
    calendar->SetToNow();
    settings->Values->Insert("TaskStatus", "Completed at " + timestampFormatter->Format(calendar->GetDateTime()));
    // No deferral is held on taskInstance because we are returning immediately.
}

/// <summary>
/// Handles background task cancellation.
/// </summary>
/// <param name="sender">The background task instance being cancelled.</param>
/// <param name="reason">The cancellation reason.</param>
void ActivitySensorBackgroundTask::OnCanceled(IBackgroundTaskInstance^ /*taskInstance*/, BackgroundTaskCancellationReason reason)
{
    auto settings = ApplicationData::Current->LocalSettings;
    auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");

    auto calendar = ref new Calendar();
    calendar->SetToNow();
    settings->Values->Insert("TaskStatus", reason.ToString() + " at " + timestampFormatter->Format(calendar->GetDateTime()));
}
