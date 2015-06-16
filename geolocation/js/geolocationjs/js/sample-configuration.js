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
        { url: "/html/scenario5_GeofenceBackgroundTask.html", title: "Background Geofencing" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();