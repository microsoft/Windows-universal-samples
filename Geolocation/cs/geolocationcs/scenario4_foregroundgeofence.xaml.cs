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
using System.Threading;
using System.Collections.Generic;
using Windows.Devices.Geolocation;
using Windows.Devices.Geolocation.Geofencing;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Globalization.DateTimeFormatting;
using Windows.Globalization;
using Windows.Globalization.NumberFormatting;
using System.Collections.ObjectModel;

namespace GeolocationCS
{
    public sealed partial class Scenario4 : Page
    {
        private bool nameSet = false;
        private bool latitudeSet = false;
        private bool longitudeSet = false;
        private bool radiusSet = false;
        private int secondsPerMinute = 60;
        private int secondsPerHour = 60 * 60;
        private int secondsPerDay = 24 * 60 * 60;
        private int oneHundredNanosecondsPerSecond = 10000000;
        private int defaultDwellTimeSeconds = 10;
        private const int maxEventDescriptors = 42; // Value determined by how many max length event descriptors (91 chars)
                                                    // stored as a JSON string can fit in 8K (max allowed for local settings)

        private CancellationTokenSource _cts = null;
        private IList<Geofence> geofences = new List<Geofence>();
        private ObservableCollection<GeofenceItem> geofenceCollection = null;
        private ObservableCollection<string> eventCollection = null;

        private DateTimeFormatter formatterShortDateLongTime;
        private DateTimeFormatter formatterLongTime;
        private Calendar calendar;
        private DecimalFormatter decimalFormatter;
        private CoreWindow coreWindow;

        // A pointer to the main page.
        private MainPage _rootPage = MainPage.Current;

        public Scenario4()
        {
            this.InitializeComponent();

            try
            {
                formatterShortDateLongTime = new DateTimeFormatter("{month.integer}/{day.integer}/{year.full} {hour.integer}:{minute.integer(2)}:{second.integer(2)}", new[] { "en-US" }, "US", Windows.Globalization.CalendarIdentifiers.Gregorian, Windows.Globalization.ClockIdentifiers.TwentyFourHour);
                formatterLongTime = new DateTimeFormatter("{hour.integer}:{minute.integer(2)}:{second.integer(2)}", new[] { "en-US" }, "US", Windows.Globalization.CalendarIdentifiers.Gregorian, Windows.Globalization.ClockIdentifiers.TwentyFourHour);
                calendar = new Calendar();
                decimalFormatter = new DecimalFormatter();
                geofenceCollection = new  ObservableCollection<GeofenceItem>();
                eventCollection = new ObservableCollection<string>();

                // Geofencing setup
                InitializeGeolocation();

                // using data binding to the root page collection of GeofenceItems
                RegisteredGeofenceListBox.DataContext = geofenceCollection;

                // using data binding to the root page collection of GeofenceItems associated with events
                GeofenceEventsListBox.DataContext = eventCollection;

                coreWindow = CoreWindow.GetForCurrentThread(); // this needs to be set before InitializeComponent sets up event registration for app visibility
                coreWindow.VisibilityChanged += OnVisibilityChanged;
            }
            catch (Exception ex)
            {
                // GeofenceMonitor failed in adding a geofence
                // exceptions could be from out of memory, lat/long out of range,
                // too long a name, not a unique name, specifying an activation
                // time + duration that is still in the past
                _rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private async void InitializeGeolocation()
        {
            // Get permission to use location
            var accessStatus = await Geolocator.RequestAccessAsync();
            switch (accessStatus)
            {
                case GeolocationAccessStatus.Allowed:
                    geofences = GeofenceMonitor.Current.Geofences;

                    FillRegisteredGeofenceListBoxWithExistingGeofences();
                    FillEventListBoxWithExistingEvents();

                    // register for state change events
                    GeofenceMonitor.Current.GeofenceStateChanged += OnGeofenceStateChanged;
                    GeofenceMonitor.Current.StatusChanged += OnGeofenceStatusChanged;
                    break;

                case GeolocationAccessStatus.Denied:
                    _rootPage.NotifyUser("Access denied.", NotifyType.ErrorMessage);
                    break;

                case GeolocationAccessStatus.Unspecified:
                    _rootPage.NotifyUser("Unspecified error.", NotifyType.ErrorMessage);
                    break;
            }
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
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
            if (_cts != null)
            {
                _cts.Cancel();
                _cts = null;
            }

            GeofenceMonitor.Current.GeofenceStateChanged -= OnGeofenceStateChanged;
            GeofenceMonitor.Current.StatusChanged -= OnGeofenceStatusChanged;

            // save off the contents of the event collection
            SaveExistingEvents();

            base.OnNavigatingFrom(e);
        }

        private void OnVisibilityChanged(CoreWindow sender, VisibilityChangedEventArgs args)
        {
            // NOTE: After the app is no longer visible on the screen and before the app is suspended
            // you might want your app to use toast notification for any geofence activity.
            // By registering for VisibiltyChanged the app is notified when the app is no longer visible in the foreground.

            if (args.Visible)
            {
                // register for foreground events
                GeofenceMonitor.Current.GeofenceStateChanged += OnGeofenceStateChanged;
                GeofenceMonitor.Current.StatusChanged += OnGeofenceStatusChanged;
            }
            else
            {
                // unregister foreground events (let background capture events)
                GeofenceMonitor.Current.GeofenceStateChanged -= OnGeofenceStateChanged;
                GeofenceMonitor.Current.StatusChanged -= OnGeofenceStatusChanged;
            }
        }

        public async void OnGeofenceStatusChanged(GeofenceMonitor sender, object e)
        {
            var status = sender.Status;

            string eventDescription = GetTimeStampedMessage("Geofence Status Changed");
            eventDescription += " (" + status.ToString() + ")";

            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                AddEventDescription(eventDescription);
            });
        }

        public async void OnGeofenceStateChanged(GeofenceMonitor sender, object e)
        {
            var reports = sender.ReadReports();

            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                foreach (GeofenceStateChangeReport report in reports)
                {
                    GeofenceState state = report.NewState;
                    Geofence geofence = report.Geofence;
                    string eventDescription = GetTimeStampedMessage(geofence.Id) +
                                              " (" + state.ToString();

                    if (state == GeofenceState.Removed)
                    {
                        eventDescription += "/" + report.RemovalReason.ToString() + ")";
                        AddEventDescription(eventDescription);

                        // remove the geofence from the client side geofences collection
                        Remove(geofence);

                        // empty the registered geofence listbox and repopulate
                        geofenceCollection.Clear();

                        FillRegisteredGeofenceListBoxWithExistingGeofences();
                    }
                    else if (state == GeofenceState.Entered || state == GeofenceState.Exited)
                    {
                        // NOTE: You might want to write your app to take particular
                        // action based on whether the app has internet connectivity.

                        eventDescription += ")";
                        AddEventDescription(eventDescription);
                    }
                }
            });
        }
        
        /// <summary>
        /// This method removes the geofence from the client side geofences collection
        /// </summary>
        /// <param name="geofence"></param>
        private void Remove(Geofence geofence)
        {
            try
            {
                if (!geofences.Remove(geofence))
                {
                    var strMsg = "Could not find Geofence " + geofence.Id + " in the geofences collection";
                    _rootPage.NotifyUser(strMsg, NotifyType.StatusMessage);
                }
            }
            catch (Exception ex)
            {
                _rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Remove Geofence Item' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnRemoveGeofenceItem(object sender, RoutedEventArgs e)
        {
            if (null != RegisteredGeofenceListBox.SelectedItem)
            {
                // get selected item
                GeofenceItem itemToRemove = RegisteredGeofenceListBox.SelectedItem as GeofenceItem;

                var geofence = itemToRemove.Geofence;

                // remove the geofence from the client side geofences collection
                Remove(geofence);

                // empty the registered geofence listbox and repopulate
                geofenceCollection.Clear();

                FillRegisteredGeofenceListBoxWithExistingGeofences();
            }
        }

        private Geofence GenerateGeofence()
        {
            string fenceKey = new string(Id.Text.ToCharArray());

            BasicGeoposition position;
            position.Latitude = Double.Parse(Latitude.Text);
            position.Longitude = Double.Parse(Longitude.Text);
            position.Altitude = 0.0;
            double radius = Double.Parse(Radius.Text);

            // the geofence is a circular region
            Geocircle geocircle = new Geocircle(position, radius);

            bool singleUse = (bool)SingleUse.IsChecked;

            // want to listen for enter geofence, exit geofence and remove geofence events
            // you can select a subset of these event states
            MonitoredGeofenceStates mask = MonitoredGeofenceStates.Entered | MonitoredGeofenceStates.Exited | MonitoredGeofenceStates.Removed;

            TimeSpan dwellTime;
            TimeSpan duration;
            DateTimeOffset startTime;

            try
            {
                // setting up how long you need to be in geofence for enter event to fire
                if (string.Empty != DwellTime.Text)
                {
                    dwellTime = new TimeSpan(ParseTimeSpan(DwellTime.Text, defaultDwellTimeSeconds));
                }
                else
                {
                    dwellTime = new TimeSpan(ParseTimeSpan("0", defaultDwellTimeSeconds));
                }

                // setting up how long the geofence should be active
                   if (string.Empty != Duration.Text)
                {
                    duration = new TimeSpan(ParseTimeSpan(Duration.Text, 0));
                }
                else
                {
                    duration = new TimeSpan(ParseTimeSpan("0", 0));
                }

                // setting up the start time of the geofence
                if (string.Empty != StartTime.Text)
                {
                    startTime = DateTimeOffset.Parse(StartTime.Text);
                }
                else
                {
                    // if you don't set start time in C# the start time defaults to 1/1/1601
                    calendar.SetToNow();
                    startTime = calendar.GetDateTime();
                }
            }
            catch (ArgumentNullException)
            {
            }
            catch (FormatException)
            {
                _rootPage.NotifyUser("Entered value is not a valid string representation of a date and time", NotifyType.ErrorMessage);
            }
            catch (ArgumentException)
            {
                _rootPage.NotifyUser("The offset is greater than 14 hours or less than -14 hours.", NotifyType.ErrorMessage);
            }

            return new Geofence(fenceKey, geocircle, mask, singleUse, dwellTime, startTime, duration);
        }
    }
}
