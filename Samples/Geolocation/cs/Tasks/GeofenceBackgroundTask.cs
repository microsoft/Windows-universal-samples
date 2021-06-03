//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
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
using Windows.Storage;
using Windows.Devices.Geolocation;
using Windows.Devices.Geolocation.Geofencing;

namespace BackgroundTask
{
    public sealed class GeofenceBackgroundTask : IBackgroundTask
    {
        // Value determined by how many max length event descriptors (91 chars)
        // stored as a JSON string can fit in 8K (max allowed for local settings)
        private const int maxEventDescriptors = 42;

        // LocalSettings persistent data.
        IDictionary<string, object> _values = ApplicationData.Current.LocalSettings.Values;

        void IBackgroundTask.Run(IBackgroundTaskInstance taskInstance)
        {
            BackgroundTaskDeferral deferral = taskInstance.GetDeferral();

            try
            {
                // Handle geofence state change reports
                ProcessGeofenceStateChangedReports();
            }
            catch (UnauthorizedAccessException)
            {
                _values["Status"] = "Location permissions are disabled. Enable access through the settings.";
                _values.Remove("BackgroundGeofenceEventCollection");
            }
            finally
            {
                deferral.Complete();
            }
        }

        private void ProcessGeofenceStateChangedReports()
        {
            // Load the values saved from last time.
            JsonArray geofenceBackgroundEvents = Helpers.LoadSavedJson("BackgroundGeofenceEventCollection");

            // Process the state changes that have occurred since the last time we ran.
            string description = null;
            GeofenceMonitor monitor = GeofenceMonitor.Current;
            IReadOnlyList<GeofenceStateChangeReport> reports = monitor.ReadReports();

            foreach (GeofenceStateChangeReport report in reports)
            {
                description = GenerateReportDescription(report);

                // Add to the collection, trimming old events if necessary.
                if (geofenceBackgroundEvents.Count == maxEventDescriptors)
                {
                    geofenceBackgroundEvents.RemoveAt(maxEventDescriptors - 1);
                }
                geofenceBackgroundEvents.Insert(0, JsonValue.CreateStringValue(description));
            }

            int numReports = reports.Count;
            if (numReports > 0)
            {
                // Save the modified results for next time.
                _values["BackgroundGeofenceEventCollection"] = geofenceBackgroundEvents.Stringify();

                if (numReports > 1)
                {
                    description = $"There are {numReports} new geofence events";
                }
                Helpers.DisplayToast(description);
            }
        }

        private string GenerateReportDescription(GeofenceStateChangeReport report)
        {
            GeofenceState state = report.NewState;
            DateTimeOffset timestamp = report.Geoposition.Coordinate.Timestamp;

            string result = $"{report.Geofence.Id} {timestamp:G}";

            if (state == GeofenceState.Removed)
            {
                GeofenceRemovalReason reason = report.RemovalReason;
                if (reason == GeofenceRemovalReason.Expired)
                {
                    result += " (Removed/Expired)";
                }
                else if (reason == GeofenceRemovalReason.Used)
                {
                    result += " (Removed/Used)";
                }
            }
            else if (state == GeofenceState.Entered)
            {
                result += " (Entered)";
            }
            else if (state == GeofenceState.Exited)
            {
                result += " (Exited)";
            }
            return result;
        }
    }
}
