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
// ProximitySensorBackgroundTask.h
// Declaration of the ProximitySensorBackgroundTask class
//

#include "pch.h"
#include "ProximitySensorBackgroundTask.h"

using namespace Tasks;
using namespace Platform;
using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Storage;
using namespace Windows::ApplicationModel::Background;

ProximitySensorBackgroundTask::ProximitySensorBackgroundTask()
{
}

/// <summary>
/// The entry point of this background task.
/// </summary>
/// <param name="taskInstance">The current background task instance.</param>
void ProximitySensorBackgroundTask::Run(IBackgroundTaskInstance^ taskInstance)
{
    // Associate a cancellation handler with the background task.
    // Even though this task isn't performing much work, it can still be canceled.
    taskInstance->Canceled += ref new BackgroundTaskCanceledEventHandler(this, &ProximitySensorBackgroundTask::OnCanceled);

    SensorDataThresholdTriggerDetails^ triggerDetails = safe_cast<SensorDataThresholdTriggerDetails^>(taskInstance->TriggerDetails);
    if (SensorType::ProximitySensor == triggerDetails->SensorType)
    {
        // read the proximity sensor readings from the trigger details
        auto reports = ProximitySensor::GetReadingsFromTriggerDetails(triggerDetails);

        auto settings = ApplicationData::Current->LocalSettings;
        settings->Values->Insert("ReportCount", reports->Size.ToString());

        auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");

        if (reports->Size > 0)
        {
            auto lastReading = reports->GetAt(reports->Size - 1);
            auto knownTimestamp = lastReading->Timestamp;

            // save the details from the reports into application cache for the foreground app to read and update UI with
            settings->Values->Insert("LastTimestamp", timestampFormatter->Format(lastReading->Timestamp));
            settings->Values->Insert("Detected", lastReading->IsDetected.ToString());
        }

        auto calendar = ref new Calendar();
        calendar->SetToNow();
        settings->Values->Insert("TaskStatus", "Completed at " + timestampFormatter->Format(calendar->GetDateTime()));
        // No deferral is held on taskInstance because we are returning immediately.
    }
}

/// <summary>
/// Handles background task cancellation.
/// </summary>
/// <param name="sender">The background task instance being canceled.</param>
/// <param name="reason">The cancellation reason.</param>
void ProximitySensorBackgroundTask::OnCanceled(IBackgroundTaskInstance^ /*taskInstance*/, BackgroundTaskCancellationReason reason)
{
    auto settings = ApplicationData::Current->LocalSettings;
    auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");

    auto calendar = ref new Calendar();
    calendar->SetToNow();
    settings->Values->Insert("TaskStatus", reason.ToString() + " at " + timestampFormatter->Format(calendar->GetDateTime()));
}
