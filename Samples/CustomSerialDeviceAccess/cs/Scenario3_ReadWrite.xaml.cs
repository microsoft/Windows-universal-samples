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
using CustomSerialDeviceAccess;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace CustomSerialDeviceAccess
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>

    public sealed partial class Scenario3_ReadWrite : Page, IDisposable
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        // Track Read Operation
        private CancellationTokenSource ReadCancellationTokenSource;
        private Object ReadCancelLock = new Object();

        private Boolean IsReadTaskPending;
        private uint ReadBytesCounter = 0;
        DataReader DataReaderObject = null;

        // Track Write Operation
        private CancellationTokenSource WriteCancellationTokenSource;
        private Object WriteCancelLock = new Object();

        private Boolean IsWriteTaskPending;
        private uint WriteBytesCounter = 0;
        DataWriter DataWriteObject = null;

        bool WriteBytesAvailable = false;

        // Indicate if we navigate away from this page or not.
        private Boolean IsNavigatedAway;

        public Scenario3_ReadWrite()
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

            IsNavigatedAway = false;
            if (EventHandlerForDevice.Current.Device == null)
            {
                ReadWriteScollViewer.Visibility = Visibility.Collapsed;
                MainPage.Current.NotifyUser("Device is not connected", NotifyType.ErrorMessage);
            }
            else
            {
                if (EventHandlerForDevice.Current.Device.PortName != "")
                {
                    MainPage.Current.NotifyUser("Connected to " + EventHandlerForDevice.Current.Device.PortName,
                                                NotifyType.StatusMessage);
                }

                // So we can reset future tasks

                ResetReadCancellationTokenSource();
                ResetWriteCancellationTokenSource();

                UpdateReadButtonStates();
                UpdateWriteButtonStates();

                UpdateReadBytesCounterView();
                UpdateReadTimeoutView();

                UpdateWriteBytesCounterView();
                UpdateWriteTimeoutView();

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

        private void UpdateWriteBytesCounterView()
        {
            WriteBytesCounterValue.Text = " " + WriteBytesCounter.ToString() + " bytes";
        }

        private void UpdateWriteTimeoutView()
        {
            WriteTimeoutValue.Text = EventHandlerForDevice.Current.Device.WriteTimeout.TotalMilliseconds.ToString();
        }

        private void UpdateReadBytesCounterView()
        {
            ReadBytesCounterValue.Text = " " + ReadBytesCounter.ToString() + " bytes";
        }

        private void UpdateReadTimeoutView()
        {
            ReadTimeoutValue.Text = EventHandlerForDevice.Current.Device.ReadTimeout.TotalMilliseconds.ToString();
        }

        private void ReadTimeoutButton_Click(object sender, RoutedEventArgs e)
        {
            int ReadTimeoutInput = int.Parse(ReadTimeoutInputValue.Text);
            EventHandlerForDevice.Current.Device.ReadTimeout = new System.TimeSpan(ReadTimeoutInput * 10000);
            UpdateReadTimeoutView();
        }

        private void WriteTimeoutButton_Click(object sender, RoutedEventArgs e)
        {
            int WriteTimeoutInput = int.Parse(WriteTimeoutInputValue.Text);
            EventHandlerForDevice.Current.Device.WriteTimeout = new System.TimeSpan(WriteTimeoutInput * 10000);
            UpdateWriteTimeoutView();
        }

        async private void ReadButton_Click(object sender, RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                try
                {
                    rootPage.NotifyUser("Reading...", NotifyType.StatusMessage);

                    // We need to set this to true so that the buttons can be updated to disable the read button. We will not be able to
                    // update the button states until after the read completes.
                    IsReadTaskPending = true;
                    DataReaderObject = new DataReader(EventHandlerForDevice.Current.Device.InputStream);
                    UpdateReadButtonStates();

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
                    IsReadTaskPending = false;
                    DataReaderObject.DetachStream();
                    DataReaderObject = null;

                    UpdateReadButtonStates();
                }
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        private async void WriteButton_Click(object sender, RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                try
                {
                    rootPage.NotifyUser("Writing...", NotifyType.StatusMessage);

                    // We need to set this to true so that the buttons can be updated to disable the write button. We will not be able to
                    // update the button states until after the write completes.
                    IsWriteTaskPending = true;
                    DataWriteObject = new DataWriter(EventHandlerForDevice.Current.Device.OutputStream);
                    UpdateWriteButtonStates();

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
                    IsWriteTaskPending = false;
                    DataWriteObject.DetachStream();
                    DataWriteObject = null;

                    UpdateWriteButtonStates();
                }
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// Allow for one operation at a time
        /// </summary>
        private void UpdateReadButtonStates()
        {
            if (IsPerformingRead())
            {
                ReadButton.IsEnabled = false;
                ReadCancelButton.IsEnabled = true;
                ReadTimeoutButton.IsEnabled = false;
                ReadTimeoutInputValue.IsEnabled = false;
            }
            else
            {
                ReadButton.IsEnabled = true;
                ReadCancelButton.IsEnabled = false;
                ReadTimeoutButton.IsEnabled = true;
                ReadTimeoutInputValue.IsEnabled = true;
            }
        }

        private void UpdateWriteButtonStates()
        {
            if (IsPerformingWrite())
            {
                WriteButton.IsEnabled = false;
                WriteCancelButton.IsEnabled = true;
                WriteTimeoutButton.IsEnabled = false;
                WriteTimeoutInputValue.IsEnabled = false;
                WriteBytesInputValue.IsEnabled = false;
            }
            else
            {
                WriteButton.IsEnabled = true;
                WriteCancelButton.IsEnabled = false;
                WriteTimeoutButton.IsEnabled = true;
                WriteTimeoutInputValue.IsEnabled = true;
                WriteBytesInputValue.IsEnabled = true;
            }
        }

        private async Task ReadAsync(CancellationToken cancellationToken)
        {

            Task<UInt32> loadAsyncTask;

            uint ReadBufferLength = 1024;

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
                ReadBytesTextBlock.Text += DataReaderObject.ReadString(bytesRead);
                ReadBytesCounter += bytesRead;
                UpdateReadBytesCounterView();

            }
            rootPage.NotifyUser("Read completed - " + bytesRead.ToString() + " bytes were read", NotifyType.StatusMessage);
        }

        private async Task WriteAsync(CancellationToken cancellationToken)
        {

            Task<UInt32> storeAsyncTask;

            if ((WriteBytesAvailable) && (WriteBytesInputValue.Text.Length != 0))
            {
                char[] buffer = new char[WriteBytesInputValue.Text.Length];
                WriteBytesInputValue.Text.CopyTo(0, buffer, 0, WriteBytesInputValue.Text.Length);
                String InputString = new string(buffer);
                DataWriteObject.WriteString(InputString);
                WriteBytesInputValue.Text = "";

                // Don't start any IO if we canceled the task
                lock (WriteCancelLock)
                {
                    cancellationToken.ThrowIfCancellationRequested();

                    // Cancellation Token will be used so we can stop the task operation explicitly
                    // The completion function should still be called so that we can properly handle a canceled task
                    storeAsyncTask = DataWriteObject.StoreAsync().AsTask(cancellationToken);
                }

                UInt32 bytesWritten = await storeAsyncTask;
                if (bytesWritten > 0)
                {
                    WriteBytesTextBlock.Text += InputString.Substring(0, (int)bytesWritten) + '\n';
                    WriteBytesCounter += bytesWritten;
                    UpdateWriteBytesCounterView();
                }
                rootPage.NotifyUser("Write completed - " + bytesWritten.ToString() + " bytes written", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("No input received to write", NotifyType.StatusMessage);
            }

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

        /// <summary>
        /// Determines if we are reading, writing, or reading and writing.
        /// </summary>
        /// <returns>If we are doing any of the above operations, we return true; false otherwise</returns>
        private Boolean IsPerformingRead()
        {
            return (IsReadTaskPending);
        }

        private Boolean IsPerformingWrite()
        {
            return (IsWriteTaskPending);
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
                    ReadButton.IsEnabled = false;
                    ReadCancelButton.IsEnabled = false;

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
                    WriteButton.IsEnabled = false;
                    WriteCancelButton.IsEnabled = false;

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

        private void ReadCancelButton_Click(object sender, RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                CancelReadTask();
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }


        private void WriteCancelButton_Click(object sender, RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                CancelWriteTask();
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        private void WriteBytesInputValue_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (WriteBytesAvailable == false)
            {
                WriteBytesInputValue.Text = "";
            }
            WriteBytesAvailable = true;
        }

        private void WriteBytesInputValue_GotFocus(object sender, RoutedEventArgs e)
        {
            if (WriteBytesAvailable == false)
            {
                WriteBytesInputValue.Text = "";
            }
        }
    }
}
