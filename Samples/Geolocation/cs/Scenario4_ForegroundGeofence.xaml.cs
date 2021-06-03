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
using System.Threading.Tasks;
using System.Collections.Generic;
using Windows.Data.Json;
using Windows.Devices.Geolocation;
using Windows.Devices.Geolocation.Geofencing;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario4_ForegroundGeofence : Page
    {
        private TimeSpan defaultDwellTime = TimeSpan.FromSeconds(10);

        // This special DateTime value is used by the Geofence to mean "start immediately".
        private DateTimeOffset startImmediately = DateTimeOffset.FromFileTime(0);

        private CancellationTokenSource _cts = null;
        private GeofenceMonitor geofenceMonitor = null;

        // A pointer to the main page.
        private MainPage _rootPage = MainPage.Current;

        // This is the TimeSpan custom format we use for the form.
        static readonly string timeSpanFormat = @"d\:h\:m\:s";

        public Scenario4_ForegroundGeofence()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            Window.Current.VisibilityChanged += OnVisibilityChanged;

            DateTime oneHourLater = DateTime.Now.AddHours(1);
            //rjc/$$
            StartDate.Date = oneHourLater.Date;
            StartTime.Time = oneHourLater.TimeOfDay;

            // Get permission to use location
            GeolocationAccessStatus accessStatus = await Geolocator.RequestAccessAsync();
            switch (accessStatus)
            {
                case GeolocationAccessStatus.Allowed:
                    // Wrap in try/catch in case user revokes access suddenly.
                    try
                    {
                        geofenceMonitor = GeofenceMonitor.Current;
                    }
                    catch (UnauthorizedAccessException)
                    {
                        _rootPage.NotifyUser("Access denied.", NotifyType.ErrorMessage);
                    }
                    if (geofenceMonitor != null)
                    {
                        foreach (Geofence geofence in geofenceMonitor.Geofences)
                        {
                            AddGeofenceToRegisteredGeofenceListBox(geofence);
                        }

                        // register for state change events
                        geofenceMonitor.GeofenceStateChanged += OnGeofenceStateChanged;
                        geofenceMonitor.StatusChanged += OnGeofenceStatusChanged;
                    }
                    break;

                case GeolocationAccessStatus.Denied:
                    _rootPage.NotifyUser("Access denied.", NotifyType.ErrorMessage);
                    break;

                case GeolocationAccessStatus.Unspecified:
                    _rootPage.NotifyUser("Unspecified error.", NotifyType.ErrorMessage);
                    break;
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            Window.Current.VisibilityChanged -= OnVisibilityChanged;

            if (_cts != null)
            {
                _cts.Cancel();
                _cts = null;
            }
            if (geofenceMonitor != null)
            {
                geofenceMonitor.GeofenceStateChanged -= OnGeofenceStateChanged;
                geofenceMonitor.StatusChanged -= OnGeofenceStatusChanged;
            }
        }

        private void OnVisibilityChanged(object sender, VisibilityChangedEventArgs args)
        {
            // NOTE: After the app is no longer visible on the screen and before the app is suspended
            // you might want your app to use toast notification for any geofence activity.
            // By registering for VisibiltyChanged the app is notified when the app is no longer visible in the foreground.

            if (geofenceMonitor != null)
            {
                if (args.Visible)
                {
                    // register for foreground events
                    geofenceMonitor.GeofenceStateChanged += OnGeofenceStateChanged;
                    geofenceMonitor.StatusChanged += OnGeofenceStatusChanged;
                }
                else
                {
                    // unregister foreground events (let background capture events)
                    geofenceMonitor.GeofenceStateChanged -= OnGeofenceStateChanged;
                    geofenceMonitor.StatusChanged -= OnGeofenceStatusChanged;
                }
            }
        }

        public async void OnGeofenceStatusChanged(GeofenceMonitor sender, object e)
        {
            GeofenceMonitorStatus status = sender.Status;

            string eventDescription = "Geofence Status Changed " + DateTime.Now.ToString("T") + " (" + status.ToString() + ")";

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
                    string eventDescription = geofence.Id + " " + DateTime.Now.ToString("T") + " (" + state.ToString();

                    if (state == GeofenceState.Removed)
                    {
                        eventDescription += "/" + report.RemovalReason.ToString() + ")";
                        AddEventDescription(eventDescription);

                        // remove the geofence from the monitor
                        geofenceMonitor.Geofences.Remove(geofence);

                        // Remove the geofence from the list box.
                        foreach (ListBoxItem item in RegisteredGeofenceListBox.Items)
                        {
                            if (item.Tag == geofence)
                            {
                                RegisteredGeofenceListBox.Items.Remove(item);
                                break;
                            }
                        }
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
        /// This is the click handler for the 'Remove Geofence Item' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnRemoveGeofenceItem(object sender, RoutedEventArgs e)
        {
            var item = RegisteredGeofenceListBox.SelectedItem as ListBoxItem;
            if (item != null)
            {
                Geofence geofence = (Geofence)item.Tag;

                // remove the geofence from the monitor
                geofenceMonitor.Geofences.Remove(geofence);

                // and remove from the UI
                RegisteredGeofenceListBox.Items.Remove(item);
            }
        }

        private Geofence GenerateGeofence()
        {
            _rootPage.NotifyUser("", NotifyType.StatusMessage);

            string fenceKey = Id.Text;
            if (fenceKey.Length == 0)
            {
                _rootPage.NotifyUser("Name must be between 1 and 64 characters.", NotifyType.ErrorMessage);
                return null;
            }

            BasicGeoposition position;
            double radius;
            if (!ParseDoubleFromTextBox(Longitude, "Longitude", -180, 180, out position.Longitude) ||
                !ParseDoubleFromTextBox(Latitude, "Latitude", -90, 90, out position.Latitude) ||
                !ParseDoubleFromTextBox(Radius, "Radius", .1, 10018754.3, out radius))
            {
                return null;
            }

            position.Altitude = 0.0;

            // the geofence is a circular region
            Geocircle geocircle = new Geocircle(position, radius);

            bool singleUse = SingleUse.IsChecked.Value;

            // want to listen for enter geofence, exit geofence and remove geofence events
            // you can select a subset of these event states
            MonitoredGeofenceStates mask = MonitoredGeofenceStates.Entered | MonitoredGeofenceStates.Exited | MonitoredGeofenceStates.Removed;

            // Dwell time is how long you need to be in geofence to have been considered to have Entered it.
            TimeSpan dwellTime;
            if (!ParseTimeSpan(DwellTime.Text, defaultDwellTime, out dwellTime))
            {
                _rootPage.NotifyUser("Invalid Dwell Time.", NotifyType.ErrorMessage);
                return null;
            }

            // The Duration is the length of time the geofence is active.
            // Zero means "infinite duration".
            TimeSpan duration;
            if (!ParseTimeSpan(Duration.Text, TimeSpan.Zero, out duration))
            {
                _rootPage.NotifyUser("Invalid Duration.", NotifyType.ErrorMessage);
                return null;
            }

            // setting up the start time of the geofence
            DateTimeOffset startTime;
            if (StartImmediately.IsChecked.Value)
            {
                // The special "start immediately" value can be used if the duration is zero.
                // Otherwise, we get the current time as the start time.
                startTime = (duration == TimeSpan.Zero) ? startImmediately : DateTimeOffset.Now;
            }
            else
            {
                startTime = StartDate.Date + StartTime.Time;
            }

            // Let the platform detect other invalid parameter combinations.
            try
            {
                return new Geofence(fenceKey, geocircle, mask, singleUse, dwellTime, startTime, duration);
            }
            catch (Exception ex)
            {
                _rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
                return null;
            }
        }

        void OnRegisteredGeofenceListBoxSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (0 == e.AddedItems.Count)
            {
                // disable the remove button
                RemoveGeofenceItem.IsEnabled = false;
            }
            else
            {
                // enable the remove button
                RemoveGeofenceItem.IsEnabled = true;

                // update controls with the values from the newly-selected geofence
                Geofence geofence = (Geofence)(RegisteredGeofenceListBox.SelectedItem as ListBoxItem).Tag;
                RefreshControlsFromGeofence(geofence);
            }
        }

        private void RefreshControlsFromGeofence(Geofence geofence)
        {
            Id.Text = geofence.Id;

            Geocircle circle = (Geocircle)geofence.Geoshape;

            Latitude.Text = circle.Center.Latitude.ToString();
            Longitude.Text = circle.Center.Longitude.ToString();
            Radius.Text = circle.Radius.ToString();
            SingleUse.IsChecked = geofence.SingleUse;

            if (geofence.DwellTime != TimeSpan.Zero)
            {
                DwellTime.Text = geofence.DwellTime.ToString(timeSpanFormat);
            }
            else
            {
                DwellTime.Text = "";
            }

            if (geofence.Duration != TimeSpan.Zero)
            {
                Duration.Text = geofence.Duration.ToString(timeSpanFormat);
            }
            else
            {
                Duration.Text = "";
            }

            if (geofence.StartTime == startImmediately)
            {
                StartImmediately.IsChecked = true;
            }
            else
            {
                StartAtSpecificTime.IsChecked = true;
                StartDate.Date = geofence.StartTime.Date;
                StartTime.Time = geofence.StartTime.TimeOfDay;
            }
        }

        private bool ParseDoubleFromTextBox(TextBox e, string name, double minValue, double maxValue, out double value)
        {
            if (!Double.TryParse(e.Text, out value) || (value < minValue) || (value > maxValue))
            {
                _rootPage.NotifyUser($"{name} must be between {minValue} and {maxValue}", NotifyType.StatusMessage);
                return false;
            }
            return true;
        }

        // add geofence to listbox
        private void AddGeofenceToRegisteredGeofenceListBox(Geofence geofence)
        {
            Geocircle circle = (Geocircle)geofence.Geoshape;

            string text = $"{geofence.Id} ({circle.Center.Latitude:0.###}, {circle.Center.Longitude:0.###}, {circle.Radius:0.#})";
            RegisteredGeofenceListBox.Items.Insert(0, new ListBoxItem { Content = text, Tag = geofence });
        }

        /// <summary>
        /// This is the click handler for the 'Set Lat/Long to current position' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void OnSetPositionToHere(object sender, RoutedEventArgs e)
        {
            SetPositionProgressBar.Visibility = Visibility.Visible;
            SetPositionToHereButton.IsEnabled = false;
            Latitude.IsEnabled = false;
            Longitude.IsEnabled = false;

            try
            {
                // Get cancellation token
                _cts = new CancellationTokenSource();
                CancellationToken token = _cts.Token;

                // Carry out the operation
                // geolocator and location permissions are initialized and checked on page creation.
                var geolocator = new Geolocator();

                // Request a high accuracy position for better accuracy locating the geofence
                geolocator.DesiredAccuracy = PositionAccuracy.High;

                Geoposition pos = await geolocator.GetGeopositionAsync().AsTask(token);

                Latitude.Text = pos.Coordinate.Point.Position.Latitude.ToString();
                Longitude.Text = pos.Coordinate.Point.Position.Longitude.ToString();

                // clear status
                _rootPage.NotifyUser("", NotifyType.StatusMessage);
            }
            catch (TaskCanceledException)
            {
                _rootPage.NotifyUser("Task canceled", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                _rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
            finally
            {
                _cts = null;
            }

            SetPositionProgressBar.Visibility = Visibility.Collapsed;
            SetPositionToHereButton.IsEnabled = true;
            Latitude.IsEnabled = true;
            Longitude.IsEnabled = true;
        }

        /// <summary>
        /// This is the click handler for the 'Create Geofence' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnCreateGeofence(object sender, RoutedEventArgs e)
        {
            Geofence geofence = GenerateGeofence();
            if (geofence != null)
            {
                try
                {
                    // add geofence to GeofenceMonitor
                    geofenceMonitor.Geofences.Add(geofence);
                }
                catch (Exception ex)
                {
                    // GeofenceMonitor failed to add the geofence. Possible reasons include
                    // duplicate or invalid name, invalid position, having a limited-duration
                    // geofence that doesn't leave enough time for the DwellTime to trigger the geofence.
                    _rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
                    geofence = null;
                }
            }

            if (geofence != null)
            {
                // Update UI after geofence successfully added to GeofenceMonitor.
                AddGeofenceToRegisteredGeofenceListBox(geofence);
            }
        }

        private void AddEventDescription(string eventDescription)
        {
            GeofenceEventsListBox.Items.Insert(0, new ListBoxItem { Content = eventDescription });
        }

        private bool ParseTimeSpan(string field, TimeSpan defaultValue, out TimeSpan value)
        {
            value = TimeSpan.Zero;
            if (!string.IsNullOrEmpty(field))
            {
                uint[] fields = new uint[4];
                string[] rawFields = field.Split(new[] { ':' });
                int count;

                for (count = 0; count < rawFields.Length; count++)
                {
                    if (!uint.TryParse(rawFields[count], out fields[count]))
                    {
                        // Not parseable.
                        return false;
                    }
                }

                Array.Reverse(fields, 0, count);

                value = TimeSpan.FromDays(fields[3]) +
                    TimeSpan.FromHours(fields[2]) +
                    TimeSpan.FromMinutes(fields[1]) +
                    TimeSpan.FromSeconds(fields[0]);
            }

            if (value == TimeSpan.Zero)
            {
                value = defaultValue;
            }

            return true;
        }
    }
}
