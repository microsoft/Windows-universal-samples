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
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Devices.Geolocation;
using Windows.Devices.Geolocation.Geofencing;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace GeolocationCS
{
    public sealed partial class Scenario4 : Page
    {
        void OnRadiusTextChanged(object sender, TextChangedEventArgs e)
        {
            radiusSet = TextChangedHandlerDouble(false, "Radius", Radius);
            DetermineCreateGeofenceButtonEnableState();
        }

        void OnLongitudeTextChanged(object sender, TextChangedEventArgs e)
        {
            longitudeSet = TextChangedHandlerDouble(false, "Longitude", Longitude);
            DetermineCreateGeofenceButtonEnableState();
        }

        void OnLatitudeTextChanged(object sender, TextChangedEventArgs e)
        {
            latitudeSet = TextChangedHandlerDouble(false, "Latitude", Latitude);
            DetermineCreateGeofenceButtonEnableState();
        }

        void OnIdTextChanged(object sender, TextChangedEventArgs e)
        {
            // get number of characters
            int charCount = Id.Text.Length;
            nameSet = (0 != charCount);
            CharCount.Text = charCount.ToString() + " characters";
            DetermineCreateGeofenceButtonEnableState();
        }

        void OnRegisteredGeofenceListBoxSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            IList<object> list = e.AddedItems;

            if (0 == list.Count)
            {
                // disable the remove button
                RemoveGeofenceItem.IsEnabled = false;
            }
            else
            {
                // enable the remove button
                RemoveGeofenceItem.IsEnabled = true;

                // update controls with the values from this geofence item
                // get selected item
                GeofenceItem item = RegisteredGeofenceListBox.SelectedItem as GeofenceItem;
                RefreshControlsFromGeofenceItem(item);
                DetermineCreateGeofenceButtonEnableState();
            }
        }

        private void RefreshControlsFromGeofenceItem(GeofenceItem item)
        {
            if (null != item)
            {
                Id.Text = item.Id;
                Latitude.Text = item.Latitude.ToString();
                Longitude.Text = item.Longitude.ToString();
                Radius.Text = item.Radius.ToString();
                SingleUse.IsChecked = item.SingleUse;

                if (0 != item.DwellTime.Ticks)
                {
                    DwellTime.Text = item.DwellTime.ToString();
                }
                else
                {
                    DwellTime.Text = "";
                }

                if (0 != item.Duration.Ticks)
                {
                    Duration.Text = item.Duration.ToString();
                }
                else
                {
                    Duration.Text = "";
                }

                if (0 != item.StartTime.Ticks)
                {
                    DateTimeOffset dt = item.StartTime;

                    StartTime.Text = formatterShortDateLongTime.Format(dt);
                }
                else
                {
                    StartTime.Text = "";
                }

                // Update flags used to enable Create Geofence button
                OnIdTextChanged(null, null);
                OnLongitudeTextChanged(null, null);
                OnLatitudeTextChanged(null, null);
                OnRadiusTextChanged(null, null);
            }
        }

        private bool TextChangedHandlerDouble(bool nullAllowed, string name, TextBox e)
        {
            bool valueSet = false;

            try
            {
                double value = Double.Parse(e.Text);
                valueSet = true;

                // clear out status message
                _rootPage.NotifyUser("", NotifyType.StatusMessage);
            }
            catch (ArgumentNullException)
            {
                if (false == nullAllowed)
                {
                    if (null != name)
                    {
                        _rootPage.NotifyUser(name + " needs a value", NotifyType.StatusMessage);
                    }
                }
            }
            catch (FormatException)
            {
                if (null != name)
                {
                    _rootPage.NotifyUser(name + " must be a number", NotifyType.StatusMessage);
                }
            }
            catch (OverflowException)
            {
                if (null != name)
                {
                    _rootPage.NotifyUser(name + " is out of bounds", NotifyType.StatusMessage);
                }
            }
            return valueSet;
        }

        private bool TextChangedHandlerInt(bool nullAllowed, string name, TextBox e)
        {
            bool valueSet = false;
            try
            {
                int value = int.Parse(e.Text);
                valueSet = true;

                // clear out status message
                _rootPage.NotifyUser("", NotifyType.StatusMessage);
            }
            catch (ArgumentNullException)
            {
                if (false == nullAllowed)
                {
                    if (null != name)
                    {
                        _rootPage.NotifyUser(name + " needs a value", NotifyType.StatusMessage);
                    }
                }
            }
            catch (FormatException)
            {
                if (null != name)
                {
                    _rootPage.NotifyUser(name + " must be a number", NotifyType.StatusMessage);
                }
            }
            catch (OverflowException)
            {
                if (null != name)
                {
                    _rootPage.NotifyUser(name + " is out of bounds", NotifyType.StatusMessage);
                }
            }
            return valueSet;
        }

        private void SetStartTimeToNowFunction()
        {
            try
            {
                calendar.SetToNow();
                DateTimeOffset dt = calendar.GetDateTime();
                StartTime.Text = formatterShortDateLongTime.Format(dt);
            }
            catch (Exception ex)
            {
                _rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private string GetTimeStampedMessage(string eventCalled)
        {
            calendar.SetToNow();
            return eventCalled + " " + formatterLongTime.Format(calendar.GetDateTime());
        }

        // are settings available so a geofence can be created?
        private bool SettingsAvailable()
        {
            bool fSettingsAvailable = false;

            if (true == nameSet &&
                true == latitudeSet &&
                true == longitudeSet &&
                true == radiusSet)
            {
                // also need to test if data is good
                fSettingsAvailable = true;
            }

            return fSettingsAvailable;
        }

        private void DetermineCreateGeofenceButtonEnableState()
        {
            CreateGeofenceButton.IsEnabled = SettingsAvailable();
        }

        // add geofence to listbox
        private void AddGeofenceToRegisteredGeofenceListBox(Geofence geofence)
        {
            GeofenceItem item = new GeofenceItem(geofence);

            // the registered geofence listbox is data bound
            // to the collection stored in the root page
            geofenceCollection.Insert(0, item);
        }

        /// <summary>
        /// This is the click handler for the 'Set Start Time to Now' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnSetStartTimeToNow(object sender, RoutedEventArgs e)
        {
            SetStartTimeToNowFunction();
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
            try
            {
                // get lat/long/radius, the fence name (fenceKey), 
                // and other properties from controls,
                // depending on data in controls for activation time
                // and duration the appropriate
                // constructor will be used.
                Geofence geofence = GenerateGeofence();

                // Add the geofence to the GeofenceMonitor's
                // collection of fences
                geofences.Add(geofence);

                // add geofence to listbox
                AddGeofenceToRegisteredGeofenceListBox(geofence);
            }
            catch (TaskCanceledException)
            {
                _rootPage.NotifyUser("Canceled", NotifyType.StatusMessage);
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

        private void FillRegisteredGeofenceListBoxWithExistingGeofences()
        {
            foreach (Geofence geofence in geofences)
            {
                AddGeofenceToRegisteredGeofenceListBox(geofence);
            }
        }

        private void FillEventListBoxWithExistingEvents()
        {
            var settings = ApplicationData.Current.LocalSettings;
            if (settings.Values.ContainsKey("ForegroundGeofenceEventCollection"))
            {
                string geofenceEvent = settings.Values["ForegroundGeofenceEventCollection"].ToString();

                if (0 != geofenceEvent.Length)
                {
                    var events = JsonValue.Parse(geofenceEvent).GetArray();

                    // NOTE: the events are accessed in reverse order
                    // because the events were added to JSON from
                    // newer to older.  AddEventDescription() adds
                    // each new entry to the beginning of the collection.
                    for (int pos = events.Count - 1; pos >= 0; pos--)
                    {
                        var element = events.GetStringAt((uint)pos);
                        AddEventDescription(element);
                    }
                }
                settings.Values["ForegroundGeofenceEventCollection"] = null;
            }
        }

        private void SaveExistingEvents()
        {
            JsonArray jsonArray = new JsonArray();

            foreach (var eventDescriptor in eventCollection)
            {
                jsonArray.Add(JsonValue.CreateStringValue(eventDescriptor.ToString()));
            }

            string jsonString = jsonArray.Stringify();
            var settings = ApplicationData.Current.LocalSettings;
            settings.Values["ForegroundGeofenceEventCollection"] = jsonString;
        }

        private void AddEventDescription(string eventDescription)
        {
            if (eventCollection.Count == maxEventDescriptors)
            {
                eventCollection.RemoveAt(maxEventDescriptors - 1);
            }
            eventCollection.Insert(0, eventDescription);
        }

        private enum TimeComponent
        {
            day,
            hour,
            minute,
            second
        }

        private long ParseTimeSpan(string field, int defaultValue)
        {
            long timeSpanValue = 0;
            char[] delimiterChars = { ':' };
            string[] timeComponents = field.Split(delimiterChars);
            int start = 4 - timeComponents.Length;

            if (start >= 0)
            {
                int loop = 0;
                int index = start;
                for (; loop < timeComponents.Length; loop++, index++)
                {
                    TimeComponent tc = (TimeComponent)index;

                    switch (tc)
                    {
                        case TimeComponent.day:
                            timeSpanValue += (long)decimalFormatter.ParseInt(timeComponents[loop]) * secondsPerDay;
                            break;

                        case TimeComponent.hour:
                            timeSpanValue += (long)decimalFormatter.ParseInt(timeComponents[loop]) * secondsPerHour;
                            break;

                        case TimeComponent.minute:
                            timeSpanValue += (long)decimalFormatter.ParseInt(timeComponents[loop]) * secondsPerMinute;
                            break;

                        case TimeComponent.second:
                            timeSpanValue += (long)decimalFormatter.ParseInt(timeComponents[loop]);
                            break;

                        default:
                            break;
                    }
                }
            }

            if (0 == timeSpanValue)
            {
                timeSpanValue = defaultValue;
            }

            timeSpanValue *= oneHundredNanosecondsPerSecond;

            return timeSpanValue;
        }
    }
}
