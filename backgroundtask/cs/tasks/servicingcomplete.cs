//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System.Diagnostics;
using System.Threading;
using Windows.ApplicationModel.Background;
using Windows.Storage;

//
// The namespace for the background tasks.
//
namespace Tasks
{
    //
    // A background task always implements the IBackgroundTask interface.
    //
    public sealed class ServicingComplete : IBackgroundTask
    {
        volatile bool _cancelRequested = false;

        //
        // The Run method is the entry point of a background task.
        //
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            Debug.WriteLine("ServicingComplete " + taskInstance.Task.Name + " starting...");

            //
            // Associate a cancellation handler with the background task.
            //
            taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(OnCanceled);

            //
            // Do background task activity for servicing complete.
            //
            uint Progress;
            for (Progress = 0; Progress <= 100; Progress += 10)
            {
                //
                // If the cancellation handler indicated that the task was canceled, stop doing the task.
                //
                if (_cancelRequested)
                {
                    break;
                }

                //
                // Indicate progress to foreground application.
                //
                taskInstance.Progress = Progress;
            }

            var settings = ApplicationData.Current.LocalSettings;
            var key = taskInstance.Task.Name;

            //
            // Write to LocalSettings to indicate that this background task ran.
            //
            settings.Values[key] = (Progress < 100) ? "Canceled" : "Completed";
            Debug.WriteLine("ServicingComplete " + taskInstance.Task.Name + ((Progress < 100) ? " Canceled" : " Completed"));
        }

        //
        // Handles background task cancellation.
        //
        private void OnCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            //
            // Indicate that the background task is canceled.
            //
            _cancelRequested = true;

            Debug.WriteLine("ServicingComplete " + sender.Task.Name + " Cancel Requested...");
        }
    }
}
