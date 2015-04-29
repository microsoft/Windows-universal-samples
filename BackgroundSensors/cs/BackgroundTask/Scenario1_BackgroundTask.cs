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
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.ApplicationModel.Background;
using Windows.Devices.Background;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.Storage;

namespace BackgroundTask
{
    public sealed class Scenario1_BackgroundTask : IBackgroundTask, IDisposable
    {
        private Accelerometer Accelerometer;
        private BackgroundTaskDeferral Deferral;
        private ulong SampleCount;

        /// <summary> 
        /// Background task entry point.
        /// </summary> 
        /// <param name="taskInstance"></param>
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            Accelerometer = Accelerometer.GetDefault();

            if (null != Accelerometer)
            {
                SampleCount = 0;

                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                uint minReportIntervalMsecs = Accelerometer.MinimumReportInterval;
                Accelerometer.ReportInterval = minReportIntervalMsecs > 16 ? minReportIntervalMsecs : 16;

                // Subscribe to accelerometer ReadingChanged events.
                Accelerometer.ReadingChanged += new TypedEventHandler<Accelerometer, AccelerometerReadingChangedEventArgs>(ReadingChanged);

                // Take a deferral that is released when the task is completed.
                Deferral = taskInstance.GetDeferral();

                // Get notified when the task is canceled.
                taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(OnCanceled);

                // Store a setting so that the app knows that the task is running.
                ApplicationData.Current.LocalSettings.Values["IsBackgroundTaskActive"] = true;
            }
        }

        /// <summary> 
        /// Called when the background task is canceled by the app or by the system.
        /// </summary> 
        /// <param name="sender"></param>
        /// <param name="reason"></param>
        private void OnCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            ApplicationData.Current.LocalSettings.Values["TaskCancelationReason"] = reason.ToString();
            ApplicationData.Current.LocalSettings.Values["SampleCount"] = SampleCount;
            ApplicationData.Current.LocalSettings.Values["IsBackgroundTaskActive"] = false;

            if (null != Accelerometer)
            {
                Accelerometer.ReadingChanged -= new TypedEventHandler<Accelerometer, AccelerometerReadingChangedEventArgs>(ReadingChanged);
                Accelerometer.ReportInterval = 0;
            }

            // Complete the background task (this raises the OnCompleted event on the corresponding BackgroundTaskRegistration).
            Deferral.Complete();
        }

        /// <summary>
        /// Frees resources held by this background task.
        /// </summary>
        public void Dispose()
        {
        }

        /// <summary>
        /// This is the event handler for acceleroemter ReadingChanged events.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ReadingChanged(object sender, AccelerometerReadingChangedEventArgs e)
        {
            SampleCount++;

            // Save the sample count if the foreground app is visible.
            bool appVisible = (bool)ApplicationData.Current.LocalSettings.Values["IsAppVisible"];
            if (appVisible)
            {
                ApplicationData.Current.LocalSettings.Values["SampleCount"] = SampleCount;
            }
        }
    }
}
