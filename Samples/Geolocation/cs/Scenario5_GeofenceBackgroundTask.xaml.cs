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
using System.Threading;
using Windows.ApplicationModel.Background;
using Windows.Devices.Geolocation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario5_GeofenceBackgroundTask : Page
    {
        private const string BackgroundTaskName = "SampleGeofenceBackgroundTask";
        private const string BackgroundTaskEntryPoint = "BackgroundTask.GeofenceBackgroundTask";

        private IBackgroundTaskRegistration _geofenceTask = null;

        // A pointer to the main page.
        private MainPage _rootPage = MainPage.Current;

        public Scenario5_GeofenceBackgroundTask()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // See if our background task is already registered.
            _geofenceTask = MainPage.LookupBackgroundTask(BackgroundTaskName);

            if (_geofenceTask != null)
            {
                // Associate an event handler with the existing background task
                _geofenceTask.Completed += OnCompleted;

                FillEventListBoxWithExistingEvents();
            }
            UpdateButtonStates();
        }

        /// <summary>
        /// This is the click handler for the 'Register' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void RegisterBackgroundTask(object sender, RoutedEventArgs e)
        {
            // Register the background task without checking whether the user has enabled background execution.
            // If it's disabled, and then the user later enables background execution, the background task will be ready to run.
            // Create a new background task builder
            BackgroundTaskBuilder geofenceTaskBuilder = new BackgroundTaskBuilder();

            geofenceTaskBuilder.Name = BackgroundTaskName;
            geofenceTaskBuilder.TaskEntryPoint = BackgroundTaskEntryPoint;

            // Create a new location trigger
            var trigger = new LocationTrigger(LocationTriggerType.Geofence);

            // Associate the location trigger with the background task builder
            geofenceTaskBuilder.SetTrigger(trigger);

            // If it is important that there is user presence and/or
            // internet connection when OnCompleted is called
            // the following could be called before calling Register()
            // SystemCondition condition = new SystemCondition(SystemConditionType.UserPresent | SystemConditionType.InternetAvailable);
            // geofenceTaskBuilder.AddCondition(condition);

            // Register the background task
            _geofenceTask = geofenceTaskBuilder.Register();

            // Associate an event handler with the new background task
            _geofenceTask.Completed += OnCompleted;

            UpdateButtonStates();
            MainPage.CheckBackgroundAndRequestLocationAccess();
        }

        /// <summary>
        /// This is the click handler for the 'Unregister' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void UnregisterBackgroundTask(object sender, RoutedEventArgs e)
        {
            // Unregister the background task
            if (null != _geofenceTask)
            {
                _geofenceTask.Unregister(true);
                _geofenceTask.Completed -= OnCompleted;
                _geofenceTask = null;
            }

            _rootPage.NotifyUser("Geofence background task unregistered", NotifyType.StatusMessage);
            UpdateButtonStates();
        }

        /// <summary>
        /// This is the callback when background event has been handled
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void OnCompleted(IBackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs e)
        {
            // Update the UI with progress reported by the background task
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                try
                {
                    // Rethrow any exception that occurred in the background task.
                    e.CheckResult();

                    // Update the UI with the completion status of the background task
                    // The Run method of the background task sets this status.
                    MainPage.ReportSavedStatus();

                    FillEventListBoxWithExistingEvents();
                }
                catch (Exception ex)
                {
                    // The background task had an error
                    _rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
                }
            });
        }

        /// <summary>
        /// Update the enable state of the register/unregister buttons.
        /// </summary>
        private void UpdateButtonStates()
        {
            bool registered = (_geofenceTask != null);
            RegisterBackgroundTaskButton.IsEnabled = !registered;
            UnregisterBackgroundTaskButton.IsEnabled = registered;
        }

        /// <summary>
        /// Populate events list box with entries from BackgroundGeofenceEventCollection.
        /// </summary>
        private void FillEventListBoxWithExistingEvents()
        {
            MainPage.FillItemsFromSavedJson(GeofenceBackgroundEventsListBox, "BackgroundGeofenceEventCollection");
        }
    }
}

