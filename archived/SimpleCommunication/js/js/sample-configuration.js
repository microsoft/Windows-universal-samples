//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Simple Communication";

    var scenarios = [
        { url: "/html/scenario1_LowLatency.html", title: "Low Latency" },
        { url: "/html/scenario2_VideoChat.html", title: "Video Chat" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),

        formatError: function (e, prefix) {
            if (e.number) {
                WinJS.log && WinJS.log(
                    prefix +
                    e.message +
                    ", Error code: " +
                    e.number.toString(16),
                    null,
                    "error"
                );
            } else {
                WinJS.log && WinJS.log(
                    prefix +
                    e.message,
                    null,
                    "error"
                );
            }
        },

        displayStatus: function (msg) {
            WinJS.log && WinJS.log(msg, null, "status");
        },

        displayError: function (msg) {
            WinJS.log && WinJS.log(msg, null, "error");
        },

        clearLastStatus: function () {
            WinJS.log && WinJS.log("", null, "status");
        }
    });
})();