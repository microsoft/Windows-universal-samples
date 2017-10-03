//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    var sampleTitle = "GeolocationJS";

    var scenarios = [
        { url: "/html/scenario1_TrackPosition.html", title: "Track position" },
        { url: "/html/scenario2_GetPosition.html", title: "Get position" },
        { url: "/html/scenario3_BackgroundTask.html", title: "Get position in background task" },
        { url: "/html/scenario4_ForegroundGeofence.html", title: "Foreground Geofencing" },
        { url: "/html/scenario5_GeofenceBackgroundTask.html", title: "Background Geofencing" },
        { url: "/html/scenario6_GetLastVisit.html", title: "Get last visit" },
        { url: "/html/scenario7_ForegroundVisits.html", title: "Foreground visit monitoring" },
        { url: "/html/scenario8_VisitsBackgroundTask.html", title: "Background visit monitoring" },
    ];

    // Namespace abbreviations.
    var Geolocation = Windows.Devices.Geolocation;

    function toStateChangeString(stateChange) {
        switch (stateChange) {
            case Geolocation.VisitStateChange.arrived:
                return "Arrived";
            case Geolocation.VisitStateChange.departed:
                return "Departed";
            case Geolocation.VisitStateChange.otherMovement:
                return "Other Movement";
            case Geolocation.VisitStateChange.trackingLost:
                return "Tracking Lost";
            default:
                return "unknown";
        }
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        toStateChangeString: toStateChangeString
    });
})();