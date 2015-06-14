//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Account picture name JS sample";

    var scenarios = [
        { url: "/html/userDisplayName.html", title: "Get display name" },
        { url: "/html/userFirstAndLastName.html", title: "Get first and last name" },
        { url: "/html/getAccountPicture.html", title: "Get account picture" },
        { url: "/html/setAccountPicture.html", title: "Set account picture and listen for changes" }
    ];

    WinJS.Navigation.addEventListener("navigating", function (e) {
        if (e.detail && e.detail.state && e.detail.state.activatedEventArgs) {
            var uri = e.detail.state.activatedEventArgs[0].uri;
            if (uri && uri.schemeName === "ms-accountpictureprovider") {
                // This app was activated via the Account picture apps section in Settings.
                // Here you would do app-specific logic for providing the user with account picture selection UX
                setImmediate(function () {
                    WinJS.Navigation.navigate(scenarios[3].url).done();
                });
            }
        }
    });

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });
})();
