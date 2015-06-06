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
using System.Globalization;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Devices.Enumeration;
using Windows.Devices.Usb;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

namespace CustomUsbDeviceAccess
{
    /// <summary>
    /// This page demonstrates how to propertly use bulk pipes to read and write to the device.
    /// </summary>
    public sealed partial class Scenario4_BulkPipes : Page, IDisposable
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        private CancellationTokenSource cancellationTokenSource;

        private Object cancelIoLock = new Object();

        private Boolean runningReadTask;
        private Boolean runningWriteTask;
        private Boolean runningReadWriteTask;

        // Indicate if we navigate away from this page or not.
        private Boolean navigatedAway;

        private UInt32 totalBytesWritten;
        private UInt32 totalBytesRead;

        public Scenario4_BulkPipes()
        {
            totalBytesRead = 0;
            totalBytesWritten = 0;
            runningReadTask = false;
            runningWriteTask = false;
            runningReadWriteTask = false;

            this.InitializeComponent();
        }

        public void Dispose()
        {
            if (cancellationTokenSource != null)
            {
                cancellationTokenSource.Dispose();
                cancellationTokenSource = null;
            }
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        ///
        /// We will enable/disable parts of the UI if the device doesn't support it.
        /// Will will also register a callback for whenever we cancel a task because we want
        /// to prevent the user from doing anymore IO (disable buttons to prevent user IO)
        /// </summary>
        /// <param name="eventArgs">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs eventArgs)
        {
            navigatedAway = false;

            // Both the OSRFX2 and the SuperMutt use the same scenario
            // If no devices are connected, none of the scenarios will be shown and an error will be displayed
            Dictionary<DeviceType, UIElement> deviceScenarios = new Dictionary<DeviceType, UIElement>();
            deviceScenarios.Add(DeviceType.OsrFx2, GeneralScenario);
            deviceScenarios.Add(DeviceType.SuperMutt, GeneralScenario);

            Utilities.SetUpDeviceScenarios(deviceScenarios, DeviceScenarioContainer);

            // So we can reset future tasks
            ResetCancellationTokenSource();

            EventHandlerForDevice.Current.OnAppSuspendCallback = new SuspendingEventHandler(this.OnAppSuspension);

            // Reset the buttons if the app resumed and the device is reconnected
            EventHandlerForDevice.Current.OnDeviceConnected = new TypedEventHandler<EventHandlerForDevice, DeviceInformation>(this.OnDeviceConnected);

            UpdateButtonStates();
        }

        /// <summary>
        /// Cancel any on going tasks when navigating away from the page so the device is in a consistent state throughout
        /// all the scenarios
        /// </summary>
        /// <param name="eventArgs"></param>
        protected override void OnNavigatedFrom(NavigationEventArgs eventArgs)
        {
            navigatedAway = true;

            CancelAllIoTasks();

            // We don't need to worry about app suspend for this scenario anymore
            EventHandlerForDevice.Current.OnAppSuspendCallback = null;
            EventHandlerForDevice.Current.OnDeviceConnected = null;
        }

        /// <summary>
        /// Stop any pending IO operations because the device will be closed when the app suspends
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void OnAppSuspension(Object sender, SuspendingEventArgs args)
        {
            CancelAllIoTasks();
        }

        /// <summary>
        /// Reset the buttons when the device is reopened
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="onDeviceConnectedEventArgs"></param>
        private void OnDeviceConnected(EventHandlerForDevice sender, DeviceInformation onDeviceConnectedEventArgs)
        {
            UpdateButtonStates();
        }

        private async void BulkRead_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                try
                {
                    rootPage.NotifyUser("Reading...", NotifyType.StatusMessage);

                    // We need to set this to true so that the buttons can be updated to disable the read button. We will not be able to
                    // update the button states until after the read completes.
                    runningReadTask = true;
                    UpdateButtonStates();

                    // Both supported devices have the bulk in pipes on index 0
                    UInt32 bulkInPipeIndex = 0;

                    // Read as much data as possible in one packet
                    UInt32 bytesToRead = 512;

                    await BulkReadAsync(bulkInPipeIndex, bytesToRead, cancellationTokenSource.Token);
                }
                catch (OperationCanceledException /*ex*/)
                {
                    NotifyTaskCanceled();
                }
                finally
                {
                    runningReadTask = false;

                    UpdateButtonStates();
                }
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        private async void BulkWrite_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                try
                {
                    rootPage.NotifyUser("Writing...", NotifyType.StatusMessage);

                    // We need to set this to true so that the buttons can be updated to disable the write button. We will not be able to
                    // update the button states until after the write completes.
                    runningWriteTask = true;
                    UpdateButtonStates();

                    // Both supported devices have the bulk out pipes on index 0
                    UInt32 bulkOutPipeIndex = 0;

                    // Write as much data as possible in one packet
                    UInt32 bytesToWrite = 512;

                    await BulkWriteAsync(bulkOutPipeIndex, bytesToWrite, cancellationTokenSource.Token);
                }
                catch (OperationCanceledException /*ex*/)
                {
                    NotifyTaskCanceled();
                }
                finally
                {
                    runningWriteTask = false;

                    UpdateButtonStates();
                }
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        private async void BulkReadWrite_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                try
                {
                    rootPage.NotifyUser("Reading/Writing...", NotifyType.StatusMessage);

                    // We need to set this to true so that the buttons can be updated to disable the read and write button, 
                    // but enable the individual read/write buttons. We will not be able to update the button states until after the read/write completes.
                    runningReadWriteTask = true;
                    UpdateButtonStates();

                    // Both supported devices have the bulk in/out pipes on index 0
                    UInt32 bulkOutPipeIndex = 0;
                    UInt32 bulkInPipeIndex = 0;

                    // Write as much data as possible in one packet
                    UInt32 bytesToWrite = 512;

                    // Since the read/writes go on indefinitely, we need to be able to cancel both reads and writes at the same time
                    await BulkReadWriteAsync(bulkInPipeIndex, bulkOutPipeIndex, bytesToWrite);

                    // This line should not be reached unless the bulk read write stopped loops because the device was disconnected.
                    await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                        new DispatchedHandler(() =>
                        {
                            // If we navigated away from this page, do not print anything. The dispatch may be handled after
                            // we move to a different page.
                            if (!navigatedAway)
                            {
                                rootPage.NotifyUser("Device was disconnected before we could read/write to it", NotifyType.StatusMessage);
                            }
                        }));
                }
                catch (OperationCanceledException /*ex*/)
                {
                    NotifyTaskCanceled();
                }
                catch (AggregateException exceptions)
                {
                    // Check to see if the reads and writes were cancelled
                    // If there was an exception other than a cancelled task, pass it up to caller
                    foreach (Exception exception in exceptions.InnerExceptions)
                    {
                        Exception realException;

                        if (exception is AggregateException && (exception.InnerException is OperationCanceledException))
                        {
                            realException = exception.InnerException;
                        }
                        else
                        {
                            realException = exception;
                        }

                        if (realException is OperationCanceledException)
                        {
                            NotifyTaskCanceled();
                        }
                        else
                        {
                            throw;
                        }
                    }
                }
                finally
                {
                    // The read/write operations are no longer running
                    runningReadWriteTask = false;

                    UpdateButtonStates();
                }
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        private void CancelAllIoTasks_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                CancelAllIoTasks();
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// Allow for one operation at a time
        /// </summary>
        private void UpdateButtonStates()
        {
            ButtonBulkReadWrite.IsEnabled = !IsPerformingIo();
            ButtonBulkRead.IsEnabled = !runningReadWriteTask && !runningReadTask;
            ButtonBulkWrite.IsEnabled = !runningReadWriteTask && !runningWriteTask;
            ButtonCancelAllIoTasks.IsEnabled = IsPerformingIo();
        }

        /// <summary>
        /// Will write garbage data to the specified output pipe. Since writing to the device may take a while to complete,
        /// we provide the write async with a known cancellation token to cancel any pending or running writes.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be 
        /// handled at the end of the task chain.
        /// </summary>
        /// <param name="pipeIndex">Index of pipe in the list of Device.DefaultInterface.BulkOutPipes</param>
        /// <param name="bytesToWrite">Bytes of garbage data to write</param>
        /// <param name="cancellationToken">Cancellation token to associate the task with</param>
        private async Task BulkWriteAsync(UInt32 bulkPipeIndex, UInt32 bytesToWrite, CancellationToken cancellationToken)
        {
            // Create an array, all default initialized to 0, and write it to the buffer
            // The data inside the buffer will be garbage
            var arrayBuffer = new Byte[bytesToWrite];

            var stream = EventHandlerForDevice.Current.Device.DefaultInterface.BulkOutPipes[(int)bulkPipeIndex].OutputStream;

            var writer = new DataWriter(stream);
            writer.WriteBytes(arrayBuffer);

            Task<UInt32> storeAsyncTask;

            // Don't start any IO if we canceled the task
            lock (cancelIoLock)
            {
                cancellationToken.ThrowIfCancellationRequested();

                // This is where the data is flushed out to the device.
                //
                // Cancellation Token will be used so we can stop the task operation explicitly
                // The completion function should still be called so that we can properly handle a canceled task
                storeAsyncTask = writer.StoreAsync().AsTask(cancellationToken);
            }

            UInt32 bytesWritten = await storeAsyncTask;

            totalBytesWritten += bytesWritten;

            PrintTotalReadWriteBytes();
        }

        /// <summary>
        /// Will read data from the specified input pipe. The data being read is garbage data because the samples devices are giving us garbage data.
        /// We use cancellation tokens to cancel reads that are pending or running.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be 
        /// handled at the end of the task chain.
        /// </summary>
        /// <param name="pipeIndex">Index of pipe in the list of Device.DefaultInterface.BulkInPipes</param>
        /// <param name="bytesToRead">Bytes of garbage data to read</param>
        /// <param name="cancellationToken">Cancellation token to associate the task with</param>
        private async Task BulkReadAsync(UInt32 bulkPipeIndex, UInt32 bytesToRead, CancellationToken cancellationToken)
        {
            var stream = EventHandlerForDevice.Current.Device.DefaultInterface.BulkInPipes[(int)bulkPipeIndex].InputStream;

            DataReader reader = new DataReader(stream);

            Task<UInt32> loadAsyncTask;

            // Don't start any IO if we canceled the task
            lock (cancelIoLock)
            {
                cancellationToken.ThrowIfCancellationRequested();

                // Cancellation Token will be used so we can stop the task operation explicitly
                // The completion function should still be called so that we can properly handle a canceled task
                loadAsyncTask = reader.LoadAsync(bytesToRead).AsTask(cancellationToken);
            }

            UInt32 bytesRead = await loadAsyncTask;

            totalBytesRead += bytesRead;

            PrintTotalReadWriteBytes();

            // The data that is read is stored in the reader object
            // e.g. To read a string from the buffer:
            // reader.ReadString(bytesRead);   
        }

        /// <summary>
        /// A read and a write will be initiated simultaneously. Reads and writes are looped; after each read/write succeeds, another read/write is initiated.
        /// 
        /// We will check if the task has been canceled and if it is, we'll throw an exception because custom made tasks are expected
        /// to do so. We cannot rely on BulkReadAsync and BulkWriteAsync to throw task cancelled exceptions because they may complete
        /// before the task cancellation takes place.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be 
        /// handled at the end of the task chain.
        /// </summary>
        /// <param name="bulkInPipeIndex">Index of pipe in the list of Device.DefaultInterface.BulkInPipes</param>
        /// <param name="bulkOutPipeIndex">Index of pipe in the list of Device.DefaultInterface.BulkOutPipes</param>
        /// <param name="bytesToReadWrite">Bytes of garbage data to read/write</param>
        private async Task BulkReadWriteAsync(UInt32 bulkInPipeIndex, UInt32 bulkOutPipeIndex, UInt32 bytesToReadWrite)
        {
            // A parent task that will wait for the read write child tasks to compelete before this task completes
            await Task.Factory.StartNew(() =>
            {
                // Save the cancellation token now in case the cancellationTokenSource is reset while IO is going on.
                var cancellationToken = cancellationTokenSource.Token;

                // Loop reads
                var readTask = Task.Factory.StartNew(() =>
                {
                    // Keep reading until the task is cancelled
                    while (!cancellationTokenSource.IsCancellationRequested && EventHandlerForDevice.Current.IsDeviceConnected)
                    {
                        // Read and do not continue until it is finished
                        BulkReadAsync(bulkInPipeIndex, bytesToReadWrite, cancellationToken).Wait();
                    }
                }, cancellationTokenSource.Token, TaskCreationOptions.AttachedToParent, TaskScheduler.Current);

                // loop writes
                var writeTask = Task.Factory.StartNew(() =>
                {
                    // Keep writing until the task is cancelled
                    while (!cancellationTokenSource.IsCancellationRequested && EventHandlerForDevice.Current.IsDeviceConnected)
                    {
                        // Write and do not continue until it is finished
                        BulkWriteAsync(bulkOutPipeIndex, bytesToReadWrite, cancellationToken).Wait();
                    }
                }, cancellationTokenSource.Token, TaskCreationOptions.AttachedToParent, TaskScheduler.Current);

            });
        }

        /// <summary>
        /// It is important to be able to cancel tasks that may take a while to complete. Canceling tasks is the only way to stop any pending IO
        /// operations asynchronously. If the UsbDevice is closed/deleted while there are pending IOs, the destructor will cancel all pending IO 
        /// operations.
        /// </summary>
        private void CancelAllIoTasks()
        {
            lock (cancelIoLock)
            {
                if (!cancellationTokenSource.IsCancellationRequested)
                {
                    cancellationTokenSource.Cancel();

                    // Existing IO already has a local copy of the old cancellation token so this reset won't affect it
                    ResetCancellationTokenSource();
                }
            }
        }

        /// <summary>
        /// Determines if we are reading, writing, or reading and writing.
        /// </summary>
        /// <returns>If we are doing any of the above operations, we return true; false otherwise</returns>
        private Boolean IsPerformingIo()
        {
            return (runningReadTask || runningWriteTask || runningReadWriteTask);
        }

        private async void PrintTotalReadWriteBytes()
        {
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Low,
                new DispatchedHandler(() =>
                {
                    // If we navigated away from this page, do not print anything. The dispatch may be handled after
                    // we move to a different page.
                    if (!navigatedAway)
                    {
                        rootPage.NotifyUser(
                            "Total bytes read: " + totalBytesRead.ToString("D", NumberFormatInfo.InvariantInfo) + "; Total bytes written: "
                            + totalBytesWritten.ToString("D", NumberFormatInfo.InvariantInfo), NotifyType.StatusMessage);
                    }
                }));
        }

        private void ResetCancellationTokenSource()
        {
            // Create a new cancellation token source so that can cancel all the tokens again
            cancellationTokenSource = new CancellationTokenSource();

            // Hook the cancellation callback (called whenever Task.cancel is called)
            cancellationTokenSource.Token.Register(() => NotifyCancelingTask());
        }

        /// <summary>
        /// Print a status message saying we are canceling a task and disable all buttons to prevent multiple cancel requests.
        /// <summary>
        private async void NotifyCancelingTask()
        {
            // Setting the dispatcher priority to high allows the UI to handle disabling of all the buttons
            // before any of the IO completion callbacks get a chance to modify the UI; that way this method
            // will never get the opportunity to overwrite UI changes made by IO callbacks
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.High,
                new DispatchedHandler(() =>
                {
                    ButtonBulkRead.IsEnabled = false;
                    ButtonBulkWrite.IsEnabled = false;
                    ButtonBulkReadWrite.IsEnabled = false;
                    ButtonCancelAllIoTasks.IsEnabled = false;

                    if (!navigatedAway)
                    {
                        rootPage.NotifyUser("Canceling task... Please wait...", NotifyType.StatusMessage);
                    }
                }));
        }

        /// <summary>
        /// Notifies the UI that the operation has been cancelled
        /// </summary>
        private async void NotifyTaskCanceled()
        {
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                new DispatchedHandler(() =>
                {
                    if (!navigatedAway)
                    {
                        rootPage.NotifyUser("The read or write operation has been cancelled", NotifyType.StatusMessage);
                    }
                }));
        }
    }
}
