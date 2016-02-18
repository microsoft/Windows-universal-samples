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

using System;
using System.Diagnostics;
using Windows.ApplicationModel.Background;
using Windows.Devices.Sensors;
using Windows.Storage;

namespace Tasks
{
    public sealed class PedometerBackgroundTask : IBackgroundTask
    {
        /// <summary>
        /// The entry point of a background task.
        /// </summary>
        /// <param name="taskInstance">The current background task instance.</param>
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            // Associate a cancellation handler with the background task.
            // Even though this task isn't performing much work, it can still be cancelled.
            taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(OnCanceled);

            // Read the pedometer reports from the trigger
            SensorDataThresholdTriggerDetails triggerDetails = taskInstance.TriggerDetails as SensorDataThresholdTriggerDetails;
            // Ensure that this trigger is of Pedometer type
            if (SensorType.Pedometer == triggerDetails.SensorType)
            {
                var nThFromLast = 1;
                var reports = Pedometer.GetReadingsFromTriggerDetails(triggerDetails);
                var settings = ApplicationData.Current.LocalSettings;
                var lastReading = reports[reports.Count - nThFromLast];
                var knownTimestamp = lastReading.Timestamp;

                settings.Values["ReportCount"] = reports.Count.ToString();
                settings.Values["LastTimestamp"] = lastReading.Timestamp;

                // insert this step kind count
                settings.Values[lastReading.StepKind.ToString()] = lastReading.CumulativeSteps.ToString();

                // All reports that have the same timestamp correspond to step counts 
                // of different step kinds supported by the sensor - Parse each one of 
                // them and save in the application data
                while (nThFromLast < reports.Count)
                {
                    // get the next reading from the last
                    nThFromLast++;
                    lastReading = reports[reports.Count - nThFromLast];

                    if (knownTimestamp.UtcTicks == lastReading.Timestamp.UtcTicks)
                    {
                        // captured at the same time indicates that this step-kind data belongs to the same sample
                        // insert this step kind count
                        settings.Values[lastReading.StepKind.ToString()] = lastReading.CumulativeSteps.ToString();
                    }
                    else
                    {
                        break;
                    }
                }

                settings.Values["TaskStatus"] = "Completed at " + DateTime.Now.ToString("u");
            }
            // No deferral is held on taskInstance because we are returning immediately.
        }

        /// <summary>
        /// Handles background task cancellation.
        /// </summary>
        /// <param name="sender">The background task instance being cancelled.</param>
        /// <param name="reason">The cancellation reason.</param>
        private void OnCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            var settings = ApplicationData.Current.LocalSettings;
            settings.Values["TaskStatus"] = reason.ToString() + " at " + DateTime.Now.ToString("u");
        }
    }
}
