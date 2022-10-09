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
using System.Threading;
using Windows.ApplicationModel.Background;
using Windows.Storage;
using Windows.Devices.Geolocation;

namespace BackgroundTask
{
    public sealed class LocationBackgroundTask : IBackgroundTask
    {
        private CancellationTokenSource _cts = null;

        // LocalSettings persistent data.
        IDictionary<string, object> _values = ApplicationData.Current.LocalSettings.Values;

        async void IBackgroundTask.Run(IBackgroundTaskInstance taskInstance)
        {
            BackgroundTaskDeferral deferral = taskInstance.GetDeferral();

            try
            {
                // Associate a cancellation handler with the background task.
                taskInstance.Canceled += OnCanceled;

                // Get cancellation token
                _cts = new CancellationTokenSource();
                CancellationToken token = _cts.Token;

                // Create geolocator object
                Geolocator geolocator = new Geolocator();

                // Make the request for the current position
                Geoposition pos = await geolocator.GetGeopositionAsync().AsTask(token);

                WriteStatusToAppData($"Time: {DateTime.Now:G}");
                WriteGeolocToAppData(pos);
            }
            catch (UnauthorizedAccessException)
            {
                WriteStatusToAppData("Disabled");
                WipeGeolocDataFromAppData();
            }
            catch (Exception ex)
            {
                WriteStatusToAppData(ex.ToString());
                WipeGeolocDataFromAppData();
            }
            finally
            {
                _cts = null;
                deferral.Complete();
            }
        }

        private void WriteGeolocToAppData(Geoposition pos)
        {
            _values["Latitude"] = pos.Coordinate.Point.Position.Latitude.ToString();
            _values["Longitude"] = pos.Coordinate.Point.Position.Longitude.ToString();
            _values["Accuracy"] = pos.Coordinate.Accuracy.ToString();
        }

        private void WipeGeolocDataFromAppData()
        {
            _values.Remove("Latitude");
            _values.Remove("Longitude");
            _values.Remove("Accuracy");
        }

        private void WriteStatusToAppData(string status)
        {
            _values["Status"] = status;
        }

        private void OnCanceled(IBackgroundTaskInstance sender, BackgroundTaskCancellationReason reason)
        {
            if (_cts != null)
            {
                _cts.Cancel();
                _cts = null;
            }
        }
    }
}
