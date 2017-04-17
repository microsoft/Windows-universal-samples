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
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.Devices.Bluetooth.Background;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.System.Threading;
using Windows.Networking.Sockets;
using Windows.Devices.Bluetooth;

namespace BackgroundTasks
{
    // A background task always implements the IBackgroundTask interface.
    public sealed class RfcommServerTask : IBackgroundTask
    {
        // Networking
        private StreamSocket socket = null;
        private DataReader reader = null;
        private DataWriter writer = null;
        private BluetoothDevice remoteDevice = null;

        private BackgroundTaskDeferral deferral = null;
        private IBackgroundTaskInstance taskInstance = null;
        private BackgroundTaskCancellationReason cancelReason = BackgroundTaskCancellationReason.Abort;
        private bool cancelRequested = false;

        ThreadPoolTimer periodicTimer = null;
        /// <summary>
        /// The entry point of a background task.
        /// </summary>
        /// <param name="taskInstance">The current background task instance.</param>
        public async void Run(IBackgroundTaskInstance taskInstance)
        {
            // Get the deferral to prevent the task from closing prematurely
            deferral = taskInstance.GetDeferral();

            // Setup our onCanceled callback and progress
            this.taskInstance = taskInstance;
            this.taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(OnCanceled);
            this.taskInstance.Progress = 0;

            // Store a setting so that the app knows that the task is running. 
            ApplicationData.Current.LocalSettings.Values["IsBackgroundTaskActive"] = true;

            periodicTimer = ThreadPoolTimer.CreatePeriodicTimer(new TimerElapsedHandler(PeriodicTimerCallback), TimeSpan.FromSeconds(1));

            try
            {
                RfcommConnectionTriggerDetails details = (RfcommConnectionTriggerDetails)taskInstance.TriggerDetails;
                if (details != null)
                {
                    socket = details.Socket;
                    remoteDevice = details.RemoteDevice;
                    ApplicationData.Current.LocalSettings.Values["RemoteDeviceName"] = remoteDevice.Name;

                    writer = new DataWriter(socket.OutputStream);
                    reader = new DataReader(socket.InputStream);
                }
                else
                {
                    ApplicationData.Current.LocalSettings.Values["BackgroundTaskStatus"] = "Trigger details returned null";
                    deferral.Complete();
                }

                var result = await ReceiveDataAsync();
            }
            catch (Exception ex)
            {
                reader = null;
                writer = null;
                socket = null;
                deferral.Complete();

                Debug.WriteLine("Exception occurred while initializing the connection, hr = " + ex.HResult.ToString("X"));
            }
        }

        private void OnCanceled(IBackgroundTaskInstance taskInstance, BackgroundTaskCancellationReason reason)
        {
            cancelReason = reason;
            cancelRequested = true;

            ApplicationData.Current.LocalSettings.Values["TaskCancelationReason"] = cancelReason.ToString();
            ApplicationData.Current.LocalSettings.Values["IsBackgroundTaskActive"] = false;
            ApplicationData.Current.LocalSettings.Values["ReceivedMessage"] = "";
            // Complete the background task (this raises the OnCompleted event on the corresponding BackgroundTaskRegistration). 
            deferral.Complete();
        }

        private async Task<int> ReceiveDataAsync()
        {
            while (true)
            {
                uint readLength = await reader.LoadAsync(sizeof(uint));
                if (readLength < sizeof(uint))
                {
                    ApplicationData.Current.LocalSettings.Values["IsBackgroundTaskActive"] = false;
                    // Complete the background task (this raises the OnCompleted event on the corresponding BackgroundTaskRegistration). 
                    deferral.Complete();
                }
                uint currentLength = reader.ReadUInt32();

                readLength = await reader.LoadAsync(currentLength);
                if (readLength < currentLength)
                {
                    ApplicationData.Current.LocalSettings.Values["IsBackgroundTaskActive"] = false;
                    // Complete the background task (this raises the OnCompleted event on the corresponding BackgroundTaskRegistration). 
                    deferral.Complete();
                }
                string message = reader.ReadString(currentLength);

                ApplicationData.Current.LocalSettings.Values["ReceivedMessage"] = message;
                taskInstance.Progress += 1;
            }
        }

        /// <summary>
        /// Periodically check if there's a new message and if there is, send it over the socket 
        /// </summary>
        /// <param name="timer"></param>
        
        private async void PeriodicTimerCallback(ThreadPoolTimer timer)
        {
            if (!cancelRequested)
            {
                string message = (string)ApplicationData.Current.LocalSettings.Values["SendMessage"];
                if (!string.IsNullOrEmpty(message))
                {
                    try
                    {
                        // Make sure that the connection is still up and there is a message to send
                        if (socket != null)
                        {
                            writer.WriteUInt32((uint)message.Length);
                            writer.WriteString(message);
                            await writer.StoreAsync();

                            ApplicationData.Current.LocalSettings.Values["SendMessage"] = null;
                        }
                        else
                        {
                            cancelReason = BackgroundTaskCancellationReason.ConditionLoss;
                            deferral.Complete();
                        }
                    }
                    catch (Exception ex)
                    {
                        ApplicationData.Current.LocalSettings.Values["TaskCancelationReason"] = ex.Message;
                        ApplicationData.Current.LocalSettings.Values["SendMessage"] = null;
                        deferral.Complete();
                    }
                }
            }
            else
            {
                // Timer clean up
                periodicTimer.Cancel();
                //
                // Write to LocalSettings to indicate that this background task ran.
                //
                ApplicationData.Current.LocalSettings.Values["TaskCancelationReason"] = cancelReason.ToString();
            }
        }
    }   
}
