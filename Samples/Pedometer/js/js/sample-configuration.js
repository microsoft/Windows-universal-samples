//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Sample Title here";

    var scenarios = [
        { url: "/html/scenario1-events.html", title: "Events" },
        { url: "/html/scenario2-history.html", title: "History" },
        { url: "/html/scenario3-currentStepCount.html", title: "Current step count" },
        { url: "/html/scenario4-backgroundPedometer.html", title: "Background Pedometer" }
    ];

    var dateTimeFormatter = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter("shortdate longtime");

    function formatTimestamp(timestamp) {
        return dateTimeFormatter.format(timestamp);
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        formatTimestamp: formatTimestamp
    });
})();