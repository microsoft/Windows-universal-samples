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
    public sealed class ProximitySensorBackgroundTask : IBackgroundTask
    {
        /// <summary>
        /// The entry point of a background task.
        /// </summary>
        /// <param name="taskInstance">The current background task instance.</param>
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            Debug.WriteLine("Background " + taskInstance.Task.Name + " Starting...");

            // Associate a cancellation handler with the background task.
            // Even though this task isn't performing much work, it can still be canceled.
            taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(OnCanceled);

            // Read the sensor readings from the trigger reports
            SensorDataThresholdTriggerDetails triggerDetails = taskInstance.TriggerDetails as SensorDataThresholdTriggerDetails;
            // Ensure the type of trigger we are dealing with is of type 'ProximitySensor'
            if (SensorType.ProximitySensor == triggerDetails.SensorType)
            {
                var reports = ProximitySensor.GetReadingsFromTriggerDetails(triggerDetails);
                var settings = ApplicationData.Current.LocalSettings;
                var lastReading = reports[reports.Count - 1];

                // cache appropriate details from this trigger in application data
                settings.Values["ReportCount"] = reports.Count.ToString();
                settings.Values["LastTimestamp"] = lastReading.Timestamp.ToString("u");
                settings.Values["Detected"] = lastReading.IsDetected.ToString();
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
            Debug.WriteLine("Background " + sender.Task.Name + " Cancel Requested...");
            settings.Values["TaskStatus"] = reason.ToString() + " at " + DateTime.Now.ToString("u");
        }
    }
}
