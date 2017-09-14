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

using SDKTemplate;
using System;
using System.Collections.ObjectModel;
using Windows.ApplicationModel.Background;
using Windows.Data.Json;
using Windows.Devices.Geolocation;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace GeolocationCS
{
    public sealed partial class Scenario8 : Page
    {
        private const string SampleBackgroundTaskName = "SampleVisitBackgroundTask";
        private const string SampleBackgroundTaskEntryPoint = "BackgroundTask.VisitBackgroundTask";
        
        private IBackgroundTaskRegistration _visitTask = null;
        private ObservableCollection<string> _visitBackgroundEvents = new ObservableCollection<string>();
        

        // A pointer to the main page.
        private MainPage _rootPage = MainPage.Current;

        public Scenario8()
        {
            this.InitializeComponent();

            // using data binding to the root page collection of visitItems associated with events
            VisitBackgroundEventsListBox.DataContext = _visitBackgroundEvents;
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Loop through all background tasks to see if SampleVisitBackgroundTask is already registered
            foreach (var cur in BackgroundTaskRegistration.AllTasks)
            {
                if (cur.Value.Name == SampleBackgroundTaskName)
                {
                    _visitTask = cur.Value;
                    break;
                }
            }

            if (_visitTask != null)
            {
                FillEventListBoxWithExistingEvents();

                // Associate an event handler with the existing background task
                _visitTask.Completed += OnCompleted;

                BackgroundAccessStatus backgroundAccessStatus = BackgroundExecutionManager.GetAccessStatus();
                switch (backgroundAccessStatus)
                {
                    case BackgroundAccessStatus.AlwaysAllowed:
                    case BackgroundAccessStatus.AllowedSubjectToSystemPolicy:
                        _rootPage.NotifyUser("Background task is already registered. Waiting for next update...", NotifyType.StatusMessage);
                        break;

                    default:
                        _rootPage.NotifyUser("Background tasks may be disabled for this app", NotifyType.ErrorMessage);
                        break;
                }
                UpdateButtonStates(registered: true);
            }
            else
            {
                UpdateButtonStates(registered: false);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Register' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void RegisterBackgroundTask(object sender, RoutedEventArgs e)
        {
            // Get permission for a background task from the user. If the user has already answered once,
            // this does nothing and the user must manually update their preference via PC Settings.
            BackgroundAccessStatus backgroundAccessStatus = await BackgroundExecutionManager.RequestAccessAsync();

            // Regardless of the answer, register the background task. If the user later adds this application
            // to the lock screen, the background task will be ready to run.
            // Create a new background task builder
            BackgroundTaskBuilder visitTaskBuilder = new BackgroundTaskBuilder();

            visitTaskBuilder.Name = SampleBackgroundTaskName;
            visitTaskBuilder.TaskEntryPoint = SampleBackgroundTaskEntryPoint;

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

            UpdateButtonStates(registered: true);

            switch (backgroundAccessStatus)
            {
                case BackgroundAccessStatus.AlwaysAllowed:
                case BackgroundAccessStatus.AllowedSubjectToSystemPolicy:
                    // BackgroundTask is allowed
                    _rootPage.NotifyUser("Geofence background task registered.", NotifyType.StatusMessage);

                    // Need to request access to location
                    // This must be done with the background task registeration
                    // because the background task cannot display UI.
                    RequestLocationAccess();
                    break;

                default:
                    _rootPage.NotifyUser("Background tasks may be disabled for this app", NotifyType.ErrorMessage);
                    break;
            }
        }

        /// <summary>
        /// Get permission for location from the user. If the user has already answered once,
        /// this does nothing and the user must manually update their preference via Settings.
        /// </summary>
        private async void RequestLocationAccess()
        {
            // Request permission to access location
            var accessStatus = await Geolocator.RequestAccessAsync();

            switch (accessStatus)
            {
                case GeolocationAccessStatus.Allowed:
                    break;

                case GeolocationAccessStatus.Denied:
                    _rootPage.NotifyUser("Access to location is denied.", NotifyType.ErrorMessage);
                    break;

                default:
                case GeolocationAccessStatus.Unspecified:
                    _rootPage.NotifyUser("Unspecificed error!", NotifyType.ErrorMessage);
                    break;
            }
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
                _visitTask.Completed -= OnCompleted;
                _visitTask.Unregister(true);
                _visitTask = null;
            }

            _rootPage.NotifyUser("Visit background task unregistered", NotifyType.StatusMessage);

            ClearExistingEvents();

            UpdateButtonStates(registered: false);
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
                        // If the background task threw an exception, display the exception in
                        // the error text box.
                        e.CheckResult();

                        // Update the UI with the completion status of the background task
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
        /// 
        private async void UpdateButtonStates(bool registered)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                () =>
                {
                    RegisterBackgroundTaskButton.IsEnabled = !registered;
                    UnregisterBackgroundTaskButton.IsEnabled = registered;
                });
        }

        /// <summary>
        /// Populate events list box with entries from BackgroundVisitEventCollection.
        /// </summary>
        /// 
        private void FillEventListBoxWithExistingEvents()
        {
            var settings = ApplicationData.Current.LocalSettings;
            if (settings.Values.ContainsKey("BackgroundVisitEventCollection"))
            {
                string visitEvent = settings.Values["BackgroundVisitEventCollection"].ToString();

                if (0 != visitEvent.Length)
                {
                    // remove all entries and repopulate
                    _visitBackgroundEvents.Clear();

                    foreach (var element in JsonValue.Parse(visitEvent).GetArray())
                    {
                        _visitBackgroundEvents.Add(element.GetString());
                    }
                }
            }
        }

        /// <summary>
        /// Clear events from BackgroundVisitEventCollection.
        /// </summary>
        /// 
        private void ClearExistingEvents()
        {
            var settings = ApplicationData.Current.LocalSettings;
            settings.Values["BackgroundVisitEventCollection"] = "";

            _visitBackgroundEvents.Clear();
        }
    }
}

