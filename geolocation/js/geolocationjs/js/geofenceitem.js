//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

var GeofenceItem = WinJS.Class.define(
    function (geofence) {
        this.geofence = geofence;
    }, {
        id: {
            get: function () {
                return this.geofence.id;
            }
        },
        latitude: {
            get: function () {
                return this.geofence.geoshape.center.latitude;
            }
        },
        longitude: {
            get: function () {
                return this.geofence.geoshape.center.longitude;
            }
        },
        radius: {
            get: function () {
                return this.geofence.geoshape.radius;
            }
        },
        dwellTime: {
            get: function () {
                return this.geofence.dwellTime;
            }
        },
        singleUse: {
            get: function () {
                return this.geofence.singleUse;
            }
        },
        mask: {
            get: function () {
                return this.geofence.monitoredStates;
            }
        },
        duration: {
            get: function () {
                return this.geofence.duration;
            }
        },
        startTime: {
            get: function () {
                return this.geofence.startTime;
            }
        }
    }
);
