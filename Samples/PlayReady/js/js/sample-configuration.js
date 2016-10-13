//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "PlayReady";

    var scenarios = [
        { url: "/html/scenario1_reactive.html",  title: "Reactive License - UWP" },
        { url: "/html/scenario2_proactive.html", title: "Proactive License - UWP" },
        { url: "/html/scenario3_eme.html",       title: "Reactive License - EME" },
        { url: "/html/scenario4_eme_stop.html",  title: "Secure Stop - EME" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle + "yes",
        scenarios: new WinJS.Binding.List(scenarios),
        sampleContentURL: "http://profficialsite.origin.mediaservices.windows.net/c51358ea-9a5e-4322-8951-897d640fdfd7/tearsofsteel_4k.ism/manifest(format=mpd-time-csf)",
        sampleSimpleLicenseURL: "http://playready.directtaps.net/pr/svc/rightsmanager.asmx?UseSimpleNonPersistentLicense=1"
    });


})();