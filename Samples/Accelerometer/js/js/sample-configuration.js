//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Accelerometer JS Sample";

    var scenarios = [
        { url: "/html/scenario0_Choose.html", title: "Choose accelerometer" },
        { url: "/html/scenario1_DataEvents.html", title: "Data Events" },
        { url: "/html/scenario2_ShakeEvents.html", title: "Shake Events" },
        { url: "/html/scenario3_Polling.html", title: "Polling" },
        { url: "/html/scenario4_OrientationChanged.html", title: "OrientationChanged Handling" },
        { url: "/html/scenario5_DataEventsBatching.html", title: "Data Events Batching" }
    ];

    function setReadingText(e, reading) {
        e.innerText = "X: " + reading.accelerationX.toFixed(2) +
            ", Y: " + reading.accelerationY.toFixed(2) +
            ", Z: " + reading.accelerationZ.toFixed(2);
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        setReadingText: setReadingText,
        accelerometerReadingType: "standard"
    });
})();