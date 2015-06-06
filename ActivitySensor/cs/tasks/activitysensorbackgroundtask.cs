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
    public sealed class ActivitySensorBackgroundTask : IBackgroundTask
    {
        /// <summary>
        /// The entry point of a background task.
        /// </summary>
        /// <param name="taskInstance">The current background task instance.</param>
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            Debug.WriteLine("Background " + taskInstance.Task.Name + " Starting...");

            // Associate a cancellation handler with the background task.
            // Even though this task isn't performing much work, it can still be cancelled.
            taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(OnCanceled);

            // Read the activity reports
            ActivitySensorTriggerDetails triggerDetails = taskInstance.TriggerDetails as ActivitySensorTriggerDetails;
            var reports = triggerDetails.ReadReports();

            var settings = ApplicationData.Current.LocalSettings;
            settings.Values["ReportCount"] = reports.Count;
            if (reports.Count > 0)
            {
                var lastReading = reports[reports.Count - 1].Reading;
                settings.Values["LastActivity"] = lastReading.Activity.ToString();
                settings.Values["LastConfidence"] = lastReading.Confidence.ToString();
                settings.Values["LastTimestamp"] = lastReading.Timestamp.ToString("u");
            }
            else
            {
                settings.Values["LastActivity"] = "No data";
                settings.Values["LastConfidence"] = "No data";
                settings.Values["LastTimestamp"] = "No data";
            }

            settings.Values["TaskStatus"] = "Completed at " + DateTime.Now.ToString("u");

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
            Debug.WriteLine("Background " + sender.Task.Name + " Cancel Requested...");
            settings.Values["TaskStatus"] = reason.ToString() + " at " + DateTime.Now.ToString("u");
        }
    }
}
