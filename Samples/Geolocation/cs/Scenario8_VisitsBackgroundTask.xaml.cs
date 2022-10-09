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
using Windows.ApplicationModel.Background;
using Windows.Data.Json;
using Windows.Devices.Geolocation;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario8_VisitsBackgroundTask : Page
    {
        private const string BackgroundTaskName = "SampleVisitBackgroundTask";
        private const string BackgroundTaskEntryPoint = "BackgroundTask.VisitBackgroundTask";

        private IBackgroundTaskRegistration _visitTask = null;

        // A pointer to the main page.
        private MainPage _rootPage = MainPage.Current;

        public Scenario8_VisitsBackgroundTask()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // See if our background task is already registered.
            _visitTask = MainPage.LookupBackgroundTask(BackgroundTaskName);

            if (_visitTask != null)
            {
                // Associate an event handler with the existing background task
                _visitTask.Completed += OnCompleted;

                FillEventListBoxWithExistingEvents();
            }
            UpdateButtonStates();
        }

        /// <summary>
        /// This is the click handler for the 'Register' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void RegisterBackgroundTask(object sender, RoutedEventArgs e)
        {
            // Get permission for a background task from the user. If the user has already answered once,
            // this does nothing and the user must manually update their preference via Settings.
            BackgroundAccessStatus backgroundAccessStatus = await BackgroundExecutionManager.RequestAccessAsync();

            // Regardless of the answer, register the background task. If the user later enables background
            // execution, the background task will be ready to run.
            // Create a new background task builder
            BackgroundTaskBuilder visitTaskBuilder = new BackgroundTaskBuilder();

            visitTaskBuilder.Name = BackgroundTaskName;
            visitTaskBuilder.TaskEntryPoint = BackgroundTaskEntryPoint;

            // Create a new visit trigger
            var trigger = new GeovisitTrigger();

            // Set the desired monitoring scope.
            // For higher granularity such as venue/building level changes, choose venue.
            // For lower granularity more or less in the range of zipcode level changes, choose city.
            // Choosing Venue here as an example.
            trigger.MonitoringScope = VisitMonitoringScope.Venue;

            // Associate the trigger with the background task builder
            visitTaskBuilder.SetTrigger(trigger);

            // If it is important that there is user presence and/or
            // internet connection when OnCompleted is called
            // the following could be called before calling Register()
            // SystemCondition condition = new SystemCondition(SystemConditionType.UserPresent | SystemConditionType.InternetAvailable);
            // visitTaskBuilder.AddCondition(condition);

            // Register the background task
            _visitTask = visitTaskBuilder.Register();

            // Associate an event handler with the new background task
            _visitTask.Completed += OnCompleted;

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
            if (null != _visitTask)
            {
                _visitTask.Unregister(true);
                _visitTask.Completed -= OnCompleted;
                _visitTask = null;
            }

            _rootPage.NotifyUser("Visit background task unregistered", NotifyType.StatusMessage);
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
            bool registered = (_visitTask != null);
            RegisterBackgroundTaskButton.IsEnabled = !registered;
            UnregisterBackgroundTaskButton.IsEnabled = registered;
        }

        /// <summary>
        /// Populate events list box with entries from BackgroundVisitEventCollection.
        /// </summary>
        private void FillEventListBoxWithExistingEvents()
        {
            MainPage.FillItemsFromSavedJson(VisitBackgroundEventsListBox, "BackgroundVisitEventCollection");
        }
    }
}

