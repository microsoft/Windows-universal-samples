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
using Windows.Devices.Geolocation;
using Windows.ApplicationModel.Background;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Foundation.Collections;

namespace SDKTemplate
{
    public sealed partial class Scenario3_BackgroundTask : Page
    {
        // For background task registration
        private const string BackgroundTaskName = "SampleLocationBackgroundTask";
        private const string BackgroundTaskEntryPoint = "BackgroundTask.LocationBackgroundTask";

        private IBackgroundTaskRegistration _geolocTask = null;

        // A pointer to the main page
        private MainPage _rootPage = MainPage.Current;

        public Scenario3_BackgroundTask()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // See if our background task is already registered.
            _geolocTask = MainPage.LookupBackgroundTask(BackgroundTaskName);

            if (_geolocTask != null)
            {
                // Associate an event handler with the existing background task
                _geolocTask.Completed += OnCompleted;
            }
            UpdateButtonStates();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (_geolocTask != null)
            {
                // Remove the event handler
                _geolocTask.Completed -= OnCompleted;
            }
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
            BackgroundTaskBuilder geolocTaskBuilder = new BackgroundTaskBuilder();

            geolocTaskBuilder.Name = BackgroundTaskName;
            geolocTaskBuilder.TaskEntryPoint = BackgroundTaskEntryPoint;

            // Create a new timer triggering at a 15 minute interval
            var trigger = new TimeTrigger(15, false);

            // Associate the timer trigger with the background task builder
            geolocTaskBuilder.SetTrigger(trigger);

            // Register the background task
            _geolocTask = geolocTaskBuilder.Register();

            // Associate an event handler with the new background task
            _geolocTask.Completed += OnCompleted;

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
            if (null != _geolocTask)
            {
                _geolocTask.Unregister(true);
                _geolocTask = null;
            }

            ScenarioOutput_Latitude.Text = "No data";
            ScenarioOutput_Longitude.Text = "No data";
            ScenarioOutput_Accuracy.Text = "No data";
            UpdateButtonStates();
            _rootPage.NotifyUser("Background task unregistered", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Event handler called when the background task is completed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void OnCompleted(IBackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs e)
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

                    // Extract and display location data set by the background task if not null
                    ScenarioOutput_Latitude.Text = MainPage.LookupSavedString("Latitude", "No data");
                    ScenarioOutput_Longitude.Text = MainPage.LookupSavedString("Longitude", "No data");
                    ScenarioOutput_Accuracy.Text = MainPage.LookupSavedString("Accuracy", "No data");
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
            bool registered = (_geolocTask != null);
            RegisterBackgroundTaskButton.IsEnabled = !registered;
            UnregisterBackgroundTaskButton.IsEnabled = registered;
        }
    }
}