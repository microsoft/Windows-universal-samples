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
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;
using Windows.Devices.Enumeration;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

namespace CustomUsbDeviceAccess
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario7_SyncDevice : Page
    {
        static private DeviceUseTrigger syncBackgroundTaskTrigger = null;

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        private BackgroundTaskRegistration backgroundSyncTaskRegistration;
        private Boolean isSyncing;

        private DeviceInformation syncDeviceInformation;
        private String syncDeviceSelector;

        // Indicate if we navigate away from this page or not.
        private Boolean navigatedAway;

        /// <summary>
        /// Determine if task with given name requires background access. Windows Phone apps always require
        /// special permission to run background tasks.
        /// </summary>
        public static bool TaskRequiresBackgroundAccess()
        {
#if WINDOWS_PHONE_APP
            return true;
#else
            return false;
#endif
        }

        public Scenario7_SyncDevice()
        {
            navigatedAway = false;
            isSyncing = false;

            // Save trigger so that we may start the background task later
            // Only one instance of the trigger can exist at a time. Since the trigger does not implement
            // IDisposable, it may still be in memory when a new trigger is created.
            if (syncBackgroundTaskTrigger == null)
            {
                syncBackgroundTaskTrigger = new DeviceUseTrigger();
            }

            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// 
        /// We will enable/disable parts of the UI if the device doesn't support it.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            navigatedAway = false;

            // Both the OSRFX2 and the SuperMutt use the same scenario
            // If no devices are connected, none of the scenarios will be shown and an error will be displayed
            Dictionary<DeviceType, UIElement> deviceScenarios = new Dictionary<DeviceType, UIElement>();
            deviceScenarios.Add(DeviceType.OsrFx2, GeneralScenario);
            deviceScenarios.Add(DeviceType.SuperMutt, GeneralScenario);

            Utilities.SetUpDeviceScenarios(deviceScenarios, DeviceScenarioContainer);

            // Unregister any existing tasks that persisted; there should be none unless the app closed/crashed
            var existingSyncTask = FindSyncTask();
            if (existingSyncTask != null)
            {
                existingSyncTask.Unregister(true);
            }

            isSyncing = false;

            UpdateButtonStates();
        }

        /// <summary>
        /// Cancel the background task so the device can be reopened
        /// </summary>
        /// <param name="eventArgs"></param>
        protected override void OnNavigatedFrom(NavigationEventArgs eventArgs)
        {
            navigatedAway = true;

            CancelSyncWithDevice();
        }

        /// <summary>
        /// Search all the existing background tasks for the sync task
        /// </summary>
        /// <returns>If found, the background task registration for the sync task; else, null.</returns>
        BackgroundTaskRegistration FindSyncTask()
        {
            foreach (var backgroundTask in BackgroundTaskRegistration.AllTasks.Values)
            {
                if (backgroundTask.Name == SyncBackgroundTaskInformation.Name)
                {
                    return (BackgroundTaskRegistration)backgroundTask;
                }
            }

            return null;
        }

        private void CancelSyncWithDevice()
        {
            if (isSyncing)
            {
                backgroundSyncTaskRegistration.Unregister(true);

                backgroundSyncTaskRegistration = null;

                // We are canceling the task, so we are no longer syncing. If the task is registered but never run,
                // the cancel completion is never called
                isSyncing = false;
            }
        }

        /// <summary>
        /// Registers for background task that will write to the device.
        /// </summary>
        private async Task SetupBackgroundTask()
        {
            if (TaskRequiresBackgroundAccess())
            {
                await BackgroundExecutionManager.RequestAccessAsync();
            }

            // Create background task to write
            var backgroundTaskBuilder = new BackgroundTaskBuilder();

            backgroundTaskBuilder.Name = SyncBackgroundTaskInformation.Name;
            backgroundTaskBuilder.TaskEntryPoint = SyncBackgroundTaskInformation.TaskEntryPoint;
            backgroundTaskBuilder.SetTrigger(syncBackgroundTaskTrigger);
            backgroundSyncTaskRegistration = backgroundTaskBuilder.Register();

            // Make sure we're notified when the task completes or if there is an update
            backgroundSyncTaskRegistration.Completed += new BackgroundTaskCompletedEventHandler(OnSyncWithDeviceCompleted);
            backgroundSyncTaskRegistration.Progress += new BackgroundTaskProgressEventHandler(OnSyncWithDeviceProgress);
        }

        /// <summary>
        /// Starts the background task that syncs with the device
        /// 
        /// The trigger.RequestAsync() must be started on the UI thread because of the prompt that appears. The caller of StartSyncBackgroundTaskAsync()
        /// is responsible for running this method in the UI thread.
        /// </summary>
        /// <returns></returns>
        private Task<DeviceTriggerResult> StartSyncBackgroundTaskAsync()
        {
            // Save the current device information so that we can reopen it after syncing
            syncDeviceInformation = EventHandlerForDevice.Current.DeviceInformation;
            syncDeviceSelector = EventHandlerForDevice.Current.DeviceSelector;

            // Allow the background task to open the device and sync with it.
            // We must close the device because Background tasks need to create new handle to the device and cannot reuse the one from this app.
            // Since the device is opened exclusively, the app must close the device before the background task can use the device.
            EventHandlerForDevice.Current.CloseDevice();

            return syncBackgroundTaskTrigger.RequestAsync(syncDeviceInformation.Id).AsTask();
        }


        /// <summary>
        /// Starts the background task that will sync with the device.
        /// </summary>
        /// <returns></returns>
        private async Task SyncWithDeviceAsync()
        {
            await SetupBackgroundTask();

            var deviceTriggerResult = await StartSyncBackgroundTaskAsync();

            // Determine if we are allowed to sync
            String errorMessage = null;

            switch (deviceTriggerResult)
            {
                case DeviceTriggerResult.Allowed:
                    isSyncing = true;
                    break;

                case DeviceTriggerResult.LowBattery:
                    isSyncing = false;
                    errorMessage = "Insufficient battery to start sync";
                    break;

                case DeviceTriggerResult.DeniedByUser:
                    isSyncing = false;
                    errorMessage = "User declined the operation";
                    break;

                case DeviceTriggerResult.DeniedBySystem:
                    isSyncing = false;
                    errorMessage = "Sync operation was denied by the system";
                    break;

                default:
                    isSyncing = false;
                    errorMessage = "Failed to initiate sync";
                    break;
            }

            // Reopen device and print error message
            if (!isSyncing && errorMessage != null)
            {
                // Reopen the device once the background task is completed
                // Don't attempt to reconnect if the device failed to connect
                var isDeviceSuccessfullyConnected = await EventHandlerForDevice.Current.OpenDeviceAsync(syncDeviceInformation, syncDeviceSelector);

                if (!isDeviceSuccessfullyConnected)
                {
                    EventHandlerForDevice.Current.IsEnabledAutoReconnect = false;
                }

                syncDeviceInformation = null;
                syncDeviceSelector = null;

                rootPage.NotifyUser("Unable to sync: " + errorMessage, NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Reopen the device after the background task is done syncing. Notify the UI of how many bytes we wrote to the device.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private async void OnSyncWithDeviceCompleted(BackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs args)
        {
            isSyncing = false;

            // Exception may be thrown if an error occurs during running the background task
            args.CheckResult();

            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                new DispatchedHandler(async () =>
                {
                    // Reopen the device once the background task is completed
                    // Don't attempt to reconnect if the device failed to connect
                    var isDeviceSuccessfullyConnected = await EventHandlerForDevice.Current.OpenDeviceAsync(syncDeviceInformation, syncDeviceSelector);

                    if (!isDeviceSuccessfullyConnected)
                    {
                        EventHandlerForDevice.Current.IsEnabledAutoReconnect = false;
                    }

                    syncDeviceInformation = null;
                    syncDeviceSelector = null;

                    // Since we are navigating away, don't touch the UI; we don't care what the output/result of the background task is
                    if (!navigatedAway)
                    {
                        var taskCompleteStatus = (String)ApplicationData.Current.LocalSettings.Values[LocalSettingKeys.SyncBackgroundTaskStatus];

                        if (taskCompleteStatus == SyncBackgroundTaskInformation.TaskCompleted)
                        {
                            UInt32 totalBytesWritten = (UInt32)ApplicationData.Current.LocalSettings.Values[LocalSettingKeys.SyncBackgroundTaskResult];

                            // Set the progress bar to be completely filled in case the progress was not updated (this can happen if the app is suspended)
                            SyncProgressBar.Value = 100;

                            rootPage.NotifyUser("Sync: Wrote " + totalBytesWritten.ToString() + " bytes to the device", NotifyType.StatusMessage);
                        }
                        else if (taskCompleteStatus == SyncBackgroundTaskInformation.TaskCanceled)
                        {
                            // Reset the progress bar in case the progress was not updated (this can happen if the app is suspended)
                            SyncProgressBar.Value = 0;

                            rootPage.NotifyUser("Syncing was canceled", NotifyType.StatusMessage);
                        }

                        UpdateButtonStates();
                    }

                    // Remove all local setting values
                    ApplicationData.Current.LocalSettings.Values.Clear();
                }));

            // Unregister the background task and let the remaining task finish until completion
            if (backgroundSyncTaskRegistration != null)
            {
                backgroundSyncTaskRegistration.Unregister(false);
            }
        }

        /// <summary>
        /// Updates the UI with the progress of the sync
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private async void OnSyncWithDeviceProgress(BackgroundTaskRegistration sender, BackgroundTaskProgressEventArgs args)
        {
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                new DispatchedHandler(() =>
                {
                    SyncProgressBar.Value = args.Progress;
                }));
        }

        private async void Sync_Click(object sender, RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                rootPage.NotifyUser("Syncing in the background...", NotifyType.StatusMessage);

                // Reset progress bar 
                SyncProgressBar.Value = 0;

                await SyncWithDeviceAsync();

                UpdateButtonStates();
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        private void CancelSync_Click(object sender, RoutedEventArgs e)
        {
            if (isSyncing)
            {
                CancelSyncWithDevice();

                UpdateButtonStates();
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        private void UpdateButtonStates()
        {
            ButtonSync.IsEnabled = !isSyncing;
            ButtonCancelSync.IsEnabled = isSyncing;
        }
    }
}
