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
using Windows.ApplicationModel.Background;
using Windows.Data.Json;
using Windows.Devices.Geolocation;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public partial class MainPage : Page
    {
        public const string FEATURE_NAME = "Geolocation";

        List<Scenario> scenarios = new List<Scenario>
        {
            new Scenario() { Title = "Track position", ClassType = typeof(Scenario1_TrackPosition) },
            new Scenario() { Title = "Get position", ClassType = typeof(Scenario2_GetPosition) },
            new Scenario() { Title = "Background position", ClassType = typeof(Scenario3_BackgroundTask) },
            new Scenario() { Title = "Foreground geofencing", ClassType = typeof(Scenario4_ForegroundGeofence) },
            new Scenario() { Title = "Background geofencing", ClassType = typeof(Scenario5_GeofenceBackgroundTask) },
            new Scenario() { Title = "Get last visit", ClassType= typeof(Scenario6_GetLastVisit)},
            new Scenario() { Title = "Foreground visit monitoring", ClassType= typeof(Scenario7_ForegroundVisits)},
            new Scenario() { Title = "Background visit monitoring", ClassType=typeof(Scenario8_VisitsBackgroundTask)},
        };

        /// <summary>
        /// Look up a background task by name.
        /// If found, then also report whether the user has granted background execution access.
        /// </summary>
        public static IBackgroundTaskRegistration LookupBackgroundTask(string name)
        {
            IBackgroundTaskRegistration found = null;

            // Loop through all background tasks to find one with the matching name
            foreach (var entry in BackgroundTaskRegistration.AllTasks)
            {
                var task = entry.Value;
                if (task.Name == name)
                {
                    found = task;
                    break;
                }
            }

            if (found != null)
            {
                // Check whether the user has already granted background execution access.
                BackgroundAccessStatus backgroundAccessStatus = BackgroundExecutionManager.GetAccessStatus();

                MainPage rootPage = MainPage.Current;
                switch (backgroundAccessStatus)
                {
                    case BackgroundAccessStatus.AlwaysAllowed:
                    case BackgroundAccessStatus.AllowedSubjectToSystemPolicy:
                        rootPage.NotifyUser("Background task is already registered. Waiting for next update...", NotifyType.StatusMessage);
                        break;

                    default:
                        rootPage.NotifyUser("Background tasks may be disabled for this app", NotifyType.ErrorMessage);
                        break;
                }
            }

            return found;

        }

        /// <summary>
        /// Check and report whether the user has granted background execution access.
        /// If so, then also request location access so that the background task can do its work.
        /// </summary>
        public static async void CheckBackgroundAndRequestLocationAccess()
        {
            // Get permission for a background task from the user. If the user has already answered once,
            // this does nothing and the user must manually update their preference via Settings.
            BackgroundAccessStatus backgroundAccessStatus = await BackgroundExecutionManager.RequestAccessAsync();

            MainPage rootPage = MainPage.Current;

            switch (backgroundAccessStatus)
            {
                case BackgroundAccessStatus.AlwaysAllowed:
                case BackgroundAccessStatus.AllowedSubjectToSystemPolicy:
                    // BackgroundTask is allowed
                    rootPage.NotifyUser("Background task registered.", NotifyType.StatusMessage);

                    // Need to request access to location.
                    // This must be done when we create the background task registration
                    // because the background task cannot display UI.
                    // If the user has already answered once,
                    // this does nothing and the user must manually update their preference via Settings.
                    switch (await Geolocator.RequestAccessAsync())
                    {
                        case GeolocationAccessStatus.Allowed:
                            break;

                        case GeolocationAccessStatus.Denied:
                            rootPage.NotifyUser("Access to location is denied.", NotifyType.ErrorMessage);
                            break;

                        case GeolocationAccessStatus.Unspecified:
                            rootPage.NotifyUser("Unspecified error!", NotifyType.ErrorMessage);
                            break;
                    }
                    break;

                default:
                    rootPage.NotifyUser("Background tasks may be disabled for this app", NotifyType.ErrorMessage);
                    break;
            }
        }

        /// <summary>
        /// Look up a string in the saved LocalSettings, or a default value if no string is found.
        /// </summary>
        /// <param name="key">The key to look up</param>
        /// <param name="defaultValue">The fallback value to use if the key is not found or the value is not a string</param>
        /// <returns></returns>
        public static string LookupSavedString(string key, string defaultValue = "")
        {
            var values = Windows.Storage.ApplicationData.Current.LocalSettings.Values;
            object o;
            values.TryGetValue(key, out o);
            string s = o as string;
            return (s == null) ? defaultValue : s;
        }

        /// <summary>
        /// Look up a JSON string in the saved LocalSettings and fill the ItemsControl with the strings in it.
        /// </summary>
        /// <param name="control"></param>
        /// <param name="key"></param>
        public static void FillItemsFromSavedJson(ItemsControl control, string key)
        {
            string json = LookupSavedString(key);
            if (!string.IsNullOrEmpty(json))
            {
                // remove all entries and repopulate
                var items = control.Items;
                items.Clear();

                foreach (IJsonValue element in JsonValue.Parse(json).GetArray())
                {
                    items.Add(element.GetString());
                }
            }
        }

        /// <summary>
        /// Look up the "Status" in the saved LocalSettings and display it if present.
        /// </summary>
        public static void ReportSavedStatus()
        {
            string status = LookupSavedString("Status");
            if (!string.IsNullOrEmpty(status))
            {
                MainPage.Current.NotifyUser(status, NotifyType.StatusMessage);
            }
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }
}
