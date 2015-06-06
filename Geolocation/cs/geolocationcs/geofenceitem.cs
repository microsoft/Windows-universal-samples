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
using Windows.Devices.Geolocation;
using Windows.Devices.Geolocation.Geofencing;

namespace GeolocationCS
{
    // GeofenceItem implements IEquatable to allow
    // removal of objects in the collection
    public class GeofenceItem : IEquatable<GeofenceItem>
    {
        private Geofence _geofence;
        private string _id;

        public GeofenceItem(Geofence geofence)
        {
            this._geofence = geofence;
            this._id = geofence.Id;
        }

        public bool Equals(GeofenceItem other)
        {
            bool isEqual = false;
            if (Id == other.Id)
            {
                isEqual = true;
            }

            return isEqual;
        }

        public Geofence Geofence
        {
            get
            {
                return _geofence;
            }
        }

        public string Id
        {
            get
            {
                return _id;
            }
        }

        public double Latitude
        {
            get
            {
                Geocircle circle = _geofence.Geoshape as Geocircle;
                return circle.Center.Latitude;
            }
        }

        public double Longitude
        {
            get
            {
                Geocircle circle = _geofence.Geoshape as Geocircle;
                return circle.Center.Longitude;
            }
        }

        public double Radius
        {
            get
            {
                Geocircle circle = _geofence.Geoshape as Geocircle;
                return circle.Radius;
            }
        }

        public bool SingleUse
        {
            get
            {
                return _geofence.SingleUse;
            }
        }

        public MonitoredGeofenceStates MonitoredStates
        {
            get
            {
                return _geofence.MonitoredStates;
            }
        }

        public TimeSpan DwellTime
        {
            get
            {
                return _geofence.DwellTime;
            }
        }

        public DateTimeOffset StartTime
        {
            get
            {
                return _geofence.StartTime;
            }
        }

        public TimeSpan Duration
        {
            get
            {
                return _geofence.Duration;
            }
        }
    }
}
