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
using Windows.UI.Xaml.Controls;
using System;
using System.Threading;
using Windows.ApplicationModel.Background;
using Windows.Data.Json;
using Windows.Devices.Geolocation;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Navigation;
using System.Collections.ObjectModel;

namespace GeolocationCS
{
    public sealed partial class Scenario5 : Page
    {
        private const string SampleBackgroundTaskName = "SampleGeofenceBackgroundTask";
        private const string SampleBackgroundTaskEntryPoint = "BackgroundTask.GeofenceBackgroundTask";
        private const long oneHundredNanosecondsPerSecond = 10000000;    // conversion from 100 nano-second resolution to seconds
        
        private IBackgroundTaskRegistration _geofenceTask = null;
        private ObservableCollection<string> _geofenceBackgroundEvents = null;
        

        // A pointer to the main page.
        private MainPage _rootPage = MainPage.Current;

        public Scenario5()
        {
            this.InitializeComponent();

            // using data binding to the root page collection of GeofenceItems associated with events
            _geofenceBackgroundEvents = new ObservableCollection<string>();
            GeofenceBackgroundEventsListBox.DataContext = _geofenceBackgroundEvents;
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Loop through all background tasks to see if SampleGeofenceBackgroundTask is already registered
            foreach (var cur in BackgroundTaskRegistration.AllTasks)
            {
                if (cur.Value.Name == SampleBackgroundTaskName)
                {
                    _geofenceTask = cur.Value;
                    break;
                }
            }

            if (_geofenceTask != null)
            {
                FillEventListBoxWithExistingEvents();

                // Associate an event handler with the existing background task
                _geofenceTask.Completed += OnCompleted;

                try
                {
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
                }
                catch (Exception ex)
                {
                    _rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
                }
                UpdateButtonStates(/*registered:*/ true);
            }
            else
            {
                UpdateButtonStates(/*registered:*/ false);
            }
        }

        /// <summary>
        /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        /// </summary>
        /// <param name="e">
        /// Event data that can be examined by overriding code. The event data is representative
        /// of the navigation that will unload the current Page unless canceled. The
        /// navigation can potentially be canceled by setting e.Cancel to true.
        /// </param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// This is the click handler for the 'Register' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void RegisterBackgroundTask(object sender, RoutedEventArgs e)
        {
            try
            {
                // Get permission for a background task from the user. If the user has already answered once,
                // this does nothing and the user must manually update their preference via PC Settings.
                BackgroundAccessStatus backgroundAccessStatus = await BackgroundExecutionManager.RequestAccessAsync();

                // Regardless of the answer, register the background task. If the user later adds this application
                // to the lock screen, the background task will be ready to run.
                // Create a new background task builder
                BackgroundTaskBuilder geofenceTaskBuilder = new BackgroundTaskBuilder();

                geofenceTaskBuilder.Name = SampleBackgroundTaskName;
                geofenceTaskBuilder.TaskEntryPoint = SampleBackgroundTaskEntryPoint;

                // Create a new location trigger
                var trigger = new LocationTrigger(LocationTriggerType.Geofence);

                // Associate the locationi trigger with the background task builder
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

                UpdateButtonStates(/*registered:*/ true);

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
            catch (Exception ex)
            {
                _rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
                UpdateButtonStates(/*registered:*/ false);
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
            if (null != _geofenceTask)
            {
                _geofenceTask.Unregister(true);
                _geofenceTask = null;
            }

            _rootPage.NotifyUser("Geofence background task unregistered", NotifyType.StatusMessage);
            UpdateButtonStates(/*registered:*/ false);
        }

        /// <summary>
        /// This is the callback when background event has been handled
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void OnCompleted(IBackgroundTaskRegistration sender, BackgroundTaskCompletedEventArgs e)
        {
            if (sender != null)
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
                        // The Run method of the background task sets the LocalSettings. 
                        var settings = ApplicationData.Current.LocalSettings;

                        // get status
                        if (settings.Values.ContainsKey("Status"))
                        {
                            _rootPage.NotifyUser(settings.Values["Status"].ToString(), NotifyType.StatusMessage);
                        }

                        FillEventListBoxWithExistingEvents();
                    }
                    catch (Exception ex)
                    {
                        // The background task had an error
                        _rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
                    }
                });
            }
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
        /// Populate events list box with entries from BackgroundGeofenceEventCollection.
        /// </summary>
        /// 
        private void FillEventListBoxWithExistingEvents()
        {
            var settings = ApplicationData.Current.LocalSettings;
            if (settings.Values.ContainsKey("BackgroundGeofenceEventCollection"))
            {
                string geofenceEvent = settings.Values["BackgroundGeofenceEventCollection"].ToString();

                if (0 != geofenceEvent.Length)
                {
                    // remove all entries and repopulate
                    _geofenceBackgroundEvents.Clear();

                    var events = JsonValue.Parse(geofenceEvent).GetArray();

                    // NOTE: the events are accessed in reverse order
                    // because the events were added to JSON from
                    // newer to older.  _geofenceBackgroundEvents.Insert() adds
                    // each new entry to the beginning of the collection.
                    for (int pos = events.Count - 1; pos >= 0; pos--)
                    {
                        var element = events.GetStringAt((uint)pos);
                        _geofenceBackgroundEvents.Insert(0, element);
                    }
                }
            }
        }
    }
}

