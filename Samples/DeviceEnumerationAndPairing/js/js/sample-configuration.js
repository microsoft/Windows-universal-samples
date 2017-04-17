//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Device Enumeration and Pairing JS Sample";

    var scenarios = [
        { url: "/html/scenario1_devicepicker.html", title: "Device Picker Common Control" },
        { url: "/html/scenario2_devicewatcher.html", title: "Enumerate and Watch Devices" },
        { url: "/html/scenario3_backgrounddevicewatcher.html", title: "Enumerate and Watch Devices in a Background Task" },
        { url: "/html/scenario4_snapshot.html", title: "Enumerate Snapshot of Devices" },
        { url: "/html/scenario5_getsingledevice.html", title: "Get Single Device" },
        { url: "/html/scenario6_customfilteraddedprops.html", title: "Custom Filter with Additional Properties" },
        { url: "/html/scenario7_deviceinformationkind.html", title: "Request Specific DeviceInformationKind" },
        { url: "/html/scenario8_pairdevice.html", title: "Basic Device Pairing" },
        { url: "/html/scenario9_custompairdevice.html", title: "Custom Device Pairing" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();