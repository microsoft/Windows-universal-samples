//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "AltimeterJS";

    var scenarios = [
        { url: "/html/scenario1_DataEvents.html", title: "Data Events" },
        { url: "/html/scenario2_Polling.html", title: "Polling" }
    ];

    // This works around an issue in the Anniversary Update (1607) in which
    // Altimeter.getDefault() returns a nonfunctional altimeter if the
    // system has no altimeter. This issue does not exist in other versions
    // of Windows 10, but the workaround is harmless to use even on versions
    // which do not have this problem. The workaround returns the default
    // altimeter only after we confirm that the system has a working altimeter.

    var altimeterPromise;

    function getDefaultAltimeterAsync() {
        var Altimeter = Windows.Devices.Sensors.Altimeter;
        var ApiInformation = Windows.Foundation.Metadata.ApiInformation;

        // Create the promise only once, and cache the result.
        if (!altimeterPromise) {
            // This workaround is needed only on the Anniversary Update (universal contract 3).
            if (!ApiInformation.isApiContractPresent("Windows.Foundation.UniversalApiContract", 3) ||
                ApiInformation.isApiContractPresent("Windows.Foundation.UniversalApiContract", 4)) {
                // The current system does not require the workaround.
                altimeterPromise = WinJS.Promise.wrap(Altimeter.getDefault());
            } else {
                var deviceSelector =
                    // Find all interface classes for altimeter sensors
                    "System.Devices.InterfaceClassGuid:=\"{0E903829-FF8A-4A93-97DF-3DCBDE402288}\"" +
                    // ... that are present on the system
                    " AND System.Devices.InterfaceEnabled:=System.StructuredQueryType.Boolean#True";

                var watcher = Windows.Devices.Enumeration.DeviceInformation.createWatcher(deviceSelector, null);
                altimeterPromise = new WinJS.Promise(function init(c) {
                    watcher.addEventListener("added", function added(s) { c(Altimeter.getDefault()); });
                    watcher.addEventListener("enumerationcompleted", function enumerationcompleted() { c(null); });
                    watcher.start();
                });
                altimeterPromise.done(function stopWatcher() { watcher.stop(); });
            }
        }
        return altimeterPromise;
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        getDefaultAltimeterAsync: getDefaultAltimeterAsync
    });
})();