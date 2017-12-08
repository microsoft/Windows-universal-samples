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
// PedometerBackgroundTask.h
// Declaration of the PedometerBackgroundTask class
//

#include "pch.h"
#include "PedometerBackgroundTask.h"

using namespace Windows::Devices::Sensors;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Storage;
using namespace Windows::ApplicationModel::Background;
using namespace Tasks;

/// <summary>
/// The entry point of a background task.
/// </summary>
/// <param name="taskInstance">The current background task instance.</param>
void PedometerBackgroundTask::Run(IBackgroundTaskInstance^ taskInstance)
{
    // Associate a cancellation handler with the background task.
    // Even though this task isn't performing much work, it can still be canceled.
    taskInstance->Canceled += ref new BackgroundTaskCanceledEventHandler(this, &PedometerBackgroundTask::OnCanceled);

    SensorDataThresholdTriggerDetails^ triggerDetails = safe_cast<SensorDataThresholdTriggerDetails^>(taskInstance->TriggerDetails);
    if (SensorType::Pedometer == triggerDetails->SensorType)
    {
        // read the pedometer readings from the trigger details
        auto reports = Pedometer::GetReadingsFromTriggerDetails(triggerDetails);

        auto settings = ApplicationData::Current->LocalSettings;
        settings->Values->Insert("ReportCount", reports->Size.ToString());

        auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");

        if (reports->Size > 0)
        {
            unsigned int nThFromLast = 1;
            // get the last value from the reports to display the step counts
            auto lastReading = reports->GetAt(reports->Size - nThFromLast);
            auto knownTimestamp = lastReading->Timestamp;

            settings->Values->Insert("LastTimestamp", timestampFormatter->Format(lastReading->Timestamp));

            // insert each 'step kind' specific count to the application cache to be used by the foreground app
            settings->Values->Insert(lastReading->StepKind.ToString(), lastReading->CumulativeSteps.ToString());

            DateTime readingTimestamp(knownTimestamp);

            // All reports that have the same timestamp correspond to step counts 
            // of different step kinds supported by the sensor - Parse each one of 
            // them and save in the application data
            while (nThFromLast < reports->Size)
            {
                // get the next reading from the last
                nThFromLast++;
                lastReading = reports->GetAt(reports->Size - nThFromLast);
                readingTimestamp = lastReading->Timestamp;

                // fetch and insert all reports that has the same time stamp
                if (readingTimestamp.UniversalTime == knownTimestamp.UniversalTime)
                {
                    // insert this 'step kind' count
                    settings->Values->Insert(lastReading->StepKind.ToString(), lastReading->CumulativeSteps.ToString());
                }
                else
                {
                    break;
                }
            }
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
void PedometerBackgroundTask::OnCanceled(IBackgroundTaskInstance^ /*taskInstance*/, BackgroundTaskCancellationReason reason)
{
    auto settings = ApplicationData::Current->LocalSettings;
    auto timestampFormatter = ref new DateTimeFormatter("day month year hour minute second");

    auto calendar = ref new Calendar();
    calendar->SetToNow();
    settings->Values->Insert("TaskStatus", reason.ToString() + " at " + timestampFormatter->Format(calendar->GetDateTime()));
}
