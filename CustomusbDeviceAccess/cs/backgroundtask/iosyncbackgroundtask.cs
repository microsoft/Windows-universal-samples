//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.Devices.Background;
using Windows.Devices.Enumeration;
using Windows.Devices.Usb;
using Windows.Storage;
using Windows.Storage.Streams;

namespace BackgroundTask
{
    public sealed class IoSyncBackgroundTask : IBackgroundTask, IDisposable
    {
        // Deferral must be used to prevent the background task from terminating 
        // while an asynchronous operation is still active
        private BackgroundTaskDeferral deferral;
        private IBackgroundTaskInstance backgroundTaskInstance;

        private DeviceUseDetails deviceSyncDetails;

        private UsbDevice device;

        private CancellationTokenSource cancellationTokenSource;

        public async void Run(IBackgroundTaskInstance taskInstance)
        {
            backgroundTaskInstance = taskInstance;

            // Trigger details contain device id and arguments that are provided by the caller in the main app
            // The taskInstance can always be casted to DeviceUseDetails if this background task was started using a DeviceUseTrigger
            deviceSyncDetails = (DeviceUseDetails)taskInstance.TriggerDetails;

            deferral = taskInstance.GetDeferral();

            try
            {
                backgroundTaskInstance.Progress = 0;

                cancellationTokenSource = new CancellationTokenSource();

                taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(OnCanceled);

                // For simplicity, no error checking will be done after opening the device. Ideally, one should always
                // check if the device was successfully opened and respond accordingly. For an example on how to do this,
                // please see Scenario 1 of this sample.
                //
                // The user may also block the device via the settings charm while we are syncing (in background task). In order to deal with
                // the user changing permissions, we have to listen for DeviceAccessInformation->AccessChanged events. See EventHandlerForDevice 
                // for how to handle DeviceAccessInformation.AccessChanged event.
                OpenDevice();

                // The sample only demonstrates a bulk write for simplicity.
                // IO operations can be done after opening the device.
                // For more information on BackgroundTasks, please see the BackgroundTask sample on MSDN.
                UInt32 bytesWritten = await Task.Run(() =>
                {
                    return WriteToDeviceAsync();
                },
                cancellationTokenSource.Token);

                ApplicationData.Current.LocalSettings.Values[LocalSettingKeys.SyncBackgroundTaskResult] = bytesWritten;
                ApplicationData.Current.LocalSettings.Values[LocalSettingKeys.SyncBackgroundTaskStatus] = BackgroundTaskInformation.TaskCompleted;
            }
            catch (OperationCanceledException /*ex*/)
            {
                ApplicationData.Current.LocalSettings.Values[LocalSettingKeys.SyncBackgroundTaskResult] = 0;
                ApplicationData.Current.LocalSettings.Values[LocalSettingKeys.SyncBackgroundTaskStatus] = BackgroundTaskInformation.TaskCanceled;
            }
            finally
            {
                // Close the device because we are finished syncing and so that the app may reopen the device
                device.Dispose();

                device = null;
            }

            // Complete the background task (this raises the OnCompleted event on the corresponding BackgroundTaskRegistration)
            deferral.Complete();
        }

        public void Dispose()
        {
            if (cancellationTokenSource != null)
            {
                cancellationTokenSource.Dispose();
                cancellationTokenSource = null;
            }
        }

        private void OpenDevice()
        {
            var deviceTask = UsbDevice.FromIdAsync(deviceSyncDetails.DeviceId).AsTask(cancellationTokenSource.Token);

            deviceTask.Wait();

            device = deviceTask.Result;

            // We opened the device, so notify the app that we've completed a bit of the background task
            backgroundTaskInstance.Progress = 10;
        }

        /// <summary>
        /// Cancels opening device and the IO operation, whichever is still running
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="reason"></param>
        private void OnCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            cancellationTokenSource.Cancel();

            backgroundTaskInstance.Progress = 0;
        }

        /// <summary>
        /// This method exists to demonstrate IO with the device since that is what is expected for a sync scenario. There already exists
        /// scenarios in this sample that demonstrate how to properly perform IO with the device, please see those for more information on 
        /// performing various kinds of IO.
        /// 
        /// Writes to device's first bulkOut endpoint of the default interface and updates progress per write.
        /// When this method finishes, the progress will be 100.
        /// </summary>
        /// <returns>Total number of bytes written to the device</returns>
        private async Task<UInt32> WriteToDeviceAsync()
        {
            UInt32 totalBytesWritten = 0;

            var firstBulkOutEndpoint = device.DefaultInterface.BulkOutPipes[0];

            // Distributes the remaining progress (out of 100) among each write
            double progressIncreasePerWrite = (100 - backgroundTaskInstance.Progress) / ((double)Sync.NumberOfTimesToWrite);

            // This progress will be incremented by the more accurate progressIncreasePerWrite value and then rounded up before notifying the app
            double syncProgress = backgroundTaskInstance.Progress;

            var dataWriter = new DataWriter(firstBulkOutEndpoint.OutputStream);

            // Create an array, all default initialized to 0, and write it to the buffer
            // The data inside the buffer will be garbage
            var arrayBuffer = new Byte[Sync.BytesToWriteAtATime];

            for (UInt32 timesWritten = 0; timesWritten < Sync.NumberOfTimesToWrite; timesWritten++)
            {
                cancellationTokenSource.Token.ThrowIfCancellationRequested();

                dataWriter.WriteBytes(arrayBuffer);

                totalBytesWritten += await dataWriter.StoreAsync().AsTask(cancellationTokenSource.Token);

                syncProgress += progressIncreasePerWrite;

                // Rounding our value up because backgroundTaskInstance.progress is an unsigned int
                backgroundTaskInstance.Progress = Convert.ToUInt32(Math.Ceiling(syncProgress));
            }

            return totalBytesWritten;
        }
    }
}
