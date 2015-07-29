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
using System.Threading;

using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

using Windows.Devices.SerialCommunication;

using System.Threading.Tasks;
using Windows.Storage.Streams;

using SDKTemplate;
using SerialArduino;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SerialArduino
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_LEDTemp : Page, IDisposable
    {

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        // Track Read Operation
        private CancellationTokenSource ReadCancellationTokenSource;
        private Object ReadCancelLock = new Object();

        DataReader DataReaderObject = null;

        // Track Write Operation
        private CancellationTokenSource WriteCancellationTokenSource;
        private Object WriteCancelLock = new Object();

        DataWriter DataWriterObject = null;

        // Indicate if we navigate away from this page or not.
        private Boolean IsNavigatedAway;

        public enum LedState
        {
            LedStateOn,
            LedStateOff
        };

        public Scenario2_LEDTemp()
        {
            this.InitializeComponent();
        }

        public void Dispose()
        {
            if (ReadCancellationTokenSource != null)
            {
                ReadCancellationTokenSource.Dispose();
                ReadCancellationTokenSource = null;
            }

            if (WriteCancellationTokenSource != null)
            {
                WriteCancellationTokenSource.Dispose();
                WriteCancellationTokenSource = null;
            }
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        ///
        /// We will enable/disable parts of the UI if the device doesn't support it.
        /// </summary>
        /// <param name="eventArgs">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.Device == null)
            {
                LEDTempScrollViewer.Visibility = Visibility.Collapsed;
                MainPage.Current.NotifyUser("Device is not connected", NotifyType.ErrorMessage);
            }
            else
            {
                MainPage.Current.NotifyUser("Connected to " + EventHandlerForDevice.Current.DeviceInformation.Id,
                                            NotifyType.StatusMessage);

                // So we can reset future tasks
                ResetReadCancellationTokenSource();
                ResetWriteCancellationTokenSource();
            }
        }

        /// <summary>
        /// Cancel any on going tasks when navigating away from the page so the device is in a consistent state throughout
        /// all the scenarios
        /// </summary>
        /// <param name="eventArgs"></param>
        protected override void OnNavigatedFrom(NavigationEventArgs eventArgs)
        {
            IsNavigatedAway = true;
            CancelAllIoTasks();
        }

        /// <summary>
        /// Invoked when the LED toggle switch is toggled
        /// </summary>
        private void LED3Switch_Toggled(object sender, RoutedEventArgs e)
        {
            if (LED3Switch.IsOn == true)
            {
                WriteLedCommand("3", LedState.LedStateOn);
            }
            else
            {
                WriteLedCommand("3", LedState.LedStateOff);
            }
        }

        private void LED4Switch_Toggled(object sender, RoutedEventArgs e)
        {
            if (LED4Switch.IsOn == true)
            {
                WriteLedCommand("4", LedState.LedStateOn);
            }
            else
            {
                WriteLedCommand("4", LedState.LedStateOff);
            }
        }

        private void LED5Switch_Toggled(object sender, RoutedEventArgs e)
        {
            if (LED5Switch.IsOn == true)
            {
                WriteLedCommand("5", LedState.LedStateOn);
            }
            else
            {
                WriteLedCommand("5", LedState.LedStateOff);
            }
        }

        private void LED6Switch_Toggled(object sender, RoutedEventArgs e)
        {
            if (LED6Switch.IsOn == true)
            {
                WriteLedCommand("6", LedState.LedStateOn);
            }
            else
            {
                WriteLedCommand("6", LedState.LedStateOff);
            }
        }
        
        /// <summary>
        /// Writes a custom command to the device to turn LEDs on/off
        /// using a DataWriter to the OutputStream
        /// </summary>
        /// <param name="ledNumber"></param>
        /// <param name="ledState"></param>
        private async void WriteLedCommand(String ledNumber, LedState ledState)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                try
                {
                    rootPage.NotifyUser("Writing...", NotifyType.StatusMessage);

                    DataWriterObject = new DataWriter(EventHandlerForDevice.Current.Device.OutputStream);

                    switch (ledState)
                    {
                        case LedState.LedStateOn:
                            DataWriterObject.WriteString("ledon " + ledNumber + "\r");
                            break;

                        case LedState.LedStateOff:
                            DataWriterObject.WriteString("ledoff " + ledNumber + "\r");
                            break;

                        default:
                            break;
                    }

                    await WriteAsync(WriteCancellationTokenSource.Token);
                }
                catch (OperationCanceledException /*exception*/)
                {
                    NotifyWriteTaskCanceled();
                }
                catch (Exception exception)
                {
                    MainPage.Current.NotifyUser(exception.Message.ToString(), NotifyType.ErrorMessage);
                    Debug.WriteLine(exception.Message.ToString());
                }
                finally
                {
                    DataWriterObject.DetachStream();
                    DataWriterObject = null;
                }
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// Invoked when the Temperature Button is clicked
        /// </summary>
        private async void TemperatureButton_Click(object sender, RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                try
                {
                    rootPage.NotifyUser("Getting temperature...", NotifyType.StatusMessage);

                    DataWriterObject = new DataWriter(EventHandlerForDevice.Current.Device.OutputStream);
                    DataWriterObject.WriteString("temp\r");

                    await WriteAsync(WriteCancellationTokenSource.Token);
                }
                catch (OperationCanceledException /*exception*/)
                {
                    NotifyWriteTaskCanceled();
                }
                catch (Exception exception)
                {
                    MainPage.Current.NotifyUser(exception.Message.ToString(), NotifyType.ErrorMessage);
                    Debug.WriteLine(exception.Message.ToString());
                }
                finally
                {
                    DataWriterObject.DetachStream();
                    DataWriterObject = null;
                }

                try
                {
                    TemperatureValue.Text = String.Empty;

                    DataReaderObject = new DataReader(EventHandlerForDevice.Current.Device.InputStream);
                    await ReadAsync(ReadCancellationTokenSource.Token);
                }
                catch (OperationCanceledException /*exception*/)
                {
                    NotifyReadTaskCanceled();
                }
                catch (Exception exception)
                {
                    MainPage.Current.NotifyUser(exception.Message.ToString(), NotifyType.ErrorMessage);
                    Debug.WriteLine(exception.Message.ToString());
                }
                finally
                {
                    DataReaderObject.DetachStream();
                    DataReaderObject = null;
                }
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// Write to the output stream using a task 
        /// </summary>
        /// <param name="cancellationToken"></param>
        private async Task WriteAsync(CancellationToken cancellationToken)
        {
            Task<UInt32> storeAsyncTask;

            // Don't start any IO if we canceled the task
            lock (WriteCancelLock)
            {
                cancellationToken.ThrowIfCancellationRequested();

                // Cancellation Token will be used so we can stop the task operation explicitly
                // The completion function should still be called so that we can properly handle a canceled task
                storeAsyncTask = DataWriterObject.StoreAsync().AsTask(cancellationToken);
            }

            UInt32 bytesWritten = await storeAsyncTask;
            rootPage.NotifyUser("Write completed - " + bytesWritten.ToString() + " bytes written", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Read from the input output stream using a task 
        /// </summary>
        /// <param name="cancellationToken"></param>
        private async Task ReadAsync(CancellationToken cancellationToken)
        {
            Task<UInt32> loadAsyncTask;

            uint ReadBufferLength = 7;

            // Don't start any IO if we canceled the task
            lock (ReadCancelLock)
            {
                cancellationToken.ThrowIfCancellationRequested();

                // Cancellation Token will be used so we can stop the task operation explicitly
                // The completion function should still be called so that we can properly handle a canceled task
                DataReaderObject.InputStreamOptions = InputStreamOptions.Partial;
                loadAsyncTask = DataReaderObject.LoadAsync(ReadBufferLength).AsTask(cancellationToken);
            }

            UInt32 bytesRead = await loadAsyncTask;

            if (bytesRead > 0)
            {
                String temp = DataReaderObject.ReadString(bytesRead);
                TemperatureValue.Text = temp.Trim() + "°C";
            }

            rootPage.NotifyUser("Read completed - " + bytesRead.ToString() + " bytes were read", NotifyType.StatusMessage);
        }


        /// <summary>
        /// It is important to be able to cancel tasks that may take a while to complete. Cancelling tasks is the only way to stop any pending IO
        /// operations asynchronously. If the Serial Device is closed/deleted while there are pending IOs, the destructor will cancel all pending IO 
        /// operations.
        /// </summary>
        /// 

        private void CancelReadTask()
        {
            lock (ReadCancelLock)
            {
                if (ReadCancellationTokenSource != null)
                {
                    if (!ReadCancellationTokenSource.IsCancellationRequested)
                    {
                        ReadCancellationTokenSource.Cancel();

                        // Existing IO already has a local copy of the old cancellation token so this reset won't affect it
                        ResetReadCancellationTokenSource();
                    }
                }
            }
        }

        private void CancelWriteTask()
        {
            lock (WriteCancelLock)
            {
                if (WriteCancellationTokenSource != null)
                {
                    if (!WriteCancellationTokenSource.IsCancellationRequested)
                    {
                        WriteCancellationTokenSource.Cancel();

                        // Existing IO already has a local copy of the old cancellation token so this reset won't affect it
                        ResetWriteCancellationTokenSource();
                    }
                }
            }
        }
        private void CancelAllIoTasks()
        {
            CancelReadTask();
            CancelWriteTask();
        }

        private void ResetReadCancellationTokenSource()
        {
            // Create a new cancellation token source so that can cancel all the tokens again
            ReadCancellationTokenSource = new CancellationTokenSource();

            // Hook the cancellation callback (called whenever Task.cancel is called)
            ReadCancellationTokenSource.Token.Register(() => NotifyReadCancelingTask());
        }

        private void ResetWriteCancellationTokenSource()
        {
            // Create a new cancellation token source so that can cancel all the tokens again
            WriteCancellationTokenSource = new CancellationTokenSource();

            // Hook the cancellation callback (called whenever Task.cancel is called)
            WriteCancellationTokenSource.Token.Register(() => NotifyWriteCancelingTask());
        }

        /// <summary>
        /// Print a status message saying we are canceling a task and disable all buttons to prevent multiple cancel requests.
        /// <summary>
        private async void NotifyReadCancelingTask()
        {
            // Setting the dispatcher priority to high allows the UI to handle disabling of all the buttons
            // before any of the IO completion callbacks get a chance to modify the UI; that way this method
            // will never get the opportunity to overwrite UI changes made by IO callbacks
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.High,
                new DispatchedHandler(() =>
                {
                    if (!IsNavigatedAway)
                    {
                        rootPage.NotifyUser("Canceling Read... Please wait...", NotifyType.StatusMessage);
                    }
                }));
        }

        private async void NotifyWriteCancelingTask()
        {
            // Setting the dispatcher priority to high allows the UI to handle disabling of all the buttons
            // before any of the IO completion callbacks get a chance to modify the UI; that way this method
            // will never get the opportunity to overwrite UI changes made by IO callbacks
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.High,
                new DispatchedHandler(() =>
                {
                    if (!IsNavigatedAway)
                    {
                        rootPage.NotifyUser("Canceling Write... Please wait...", NotifyType.StatusMessage);
                    }
                }));
        }


        /// <summary>
        /// Notifies the UI that the operation has been cancelled
        /// </summary>
        private async void NotifyReadTaskCanceled()
        {
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                new DispatchedHandler(() =>
                {
                    if (!IsNavigatedAway)
                    {
                        rootPage.NotifyUser("Read request has been cancelled", NotifyType.StatusMessage);
                    }
                }));
        }

        private async void NotifyWriteTaskCanceled()
        {
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                new DispatchedHandler(() =>
                {
                    if (!IsNavigatedAway)
                    {
                        rootPage.NotifyUser("Write request has been cancelled", NotifyType.StatusMessage);
                    }
                }));
        }

    }
}
