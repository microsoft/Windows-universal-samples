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
using Windows.Devices.Geolocation;
using Windows.Globalization;
using Windows.Globalization.DateTimeFormatting;
using Windows.Storage;

namespace BackgroundTask
{
    public sealed class VisitBackgroundTask : IBackgroundTask
    {
        // Value determined by how many max length event descriptors (91 chars)
        // stored as a JSON string can fit in 8K (max allowed for local settings)
        private const int maxEventDescriptors = 42;

        // LocalSettings persistent data.
        IDictionary<string, object> _values = ApplicationData.Current.LocalSettings.Values;

        void IBackgroundTask.Run(IBackgroundTaskInstance taskInstance)
        {
            BackgroundTaskDeferral deferral = taskInstance.GetDeferral();
            GeovisitTriggerDetails triggerDetails = (GeovisitTriggerDetails)taskInstance.TriggerDetails;

            // Handle Visit reports
            ProcessVisitReports(triggerDetails);
            deferral.Complete();
        }

        private void ProcessVisitReports(GeovisitTriggerDetails triggerDetails)
        {
            // Load the values saved from last time.
            JsonArray visitBackgroundEvents = Helpers.LoadSavedJson("BackgroundVisitEventCollection");

            // Read reports from the triggerDetails.
            IReadOnlyList<Geovisit> reports = triggerDetails.ReadReports();

            string description = null;

            foreach (Geovisit report in reports)
            {
                description = GenerateVisitDescription(report);

                // Add to the collection, trimming old events if necessary.
                if (visitBackgroundEvents.Count == maxEventDescriptors)
                {
                    visitBackgroundEvents.RemoveAt(maxEventDescriptors - 1);
                }
                visitBackgroundEvents.Insert(0, JsonValue.CreateStringValue(description));
            }

            int numReports = reports.Count;
            if (numReports > 0)
            {
                // Save the modified results for next time.
                _values["BackgroundVisitEventCollection"] = visitBackgroundEvents.Stringify();

                if (numReports > 1)
                {
                    description = $"There are {numReports} new geofence events";
                }
                Helpers.DisplayToast(description);
            }
        }

        private string GenerateVisitDescription(Geovisit visit)
        {
            string result = $"{visit.Timestamp:G} {visit.StateChange}";

            // Check if the report has a valid position.
            if (visit.Position != null)
            {
                result += $" ({visit.Position.Coordinate.Point.Position.Latitude}, {visit.Position.Coordinate.Point.Position.Longitude})";
            }

            return result;
        }
    }
}
