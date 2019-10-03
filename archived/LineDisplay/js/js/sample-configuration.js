//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

(function () {
    "use strict";

    var sampleTitle = "Line Display";

    var scenarios = [
        { url: "/html/scenario1-selectDisplay.html", title: "Select a line display" },
        { url: "/html/scenario2-displayText.html", title: "Displaying text" },
        { url: "/html/scenario3-usingWindows.html", title: "Using windows to control layout" },
        { url: "/html/scenario4-updatingLineDisplayAttributes.html", title: "Updating line display attributes" },
        { url: "/html/scenario5-definingCustomGlyphs.html", title: "Defining custom glyphs" },
        { url: "/html/scenario6-manipulatingCursorAttributes.html", title: "Modifying the display cursor" },
        { url: "/html/scenario7-scrollingContentUsingMarquee.html", title: "Scrolling content using marquee" }
    ];

    async function claimScenarioLineDisplayAsync() {
        var lineDisplay = null;
        if (!SdkSample.lineDisplayId) {
            WinJS.log("You must use scenario 1 to select a line display", "sample", "error");
        } else {
            lineDisplay = await Windows.Devices.PointOfService.ClaimedLineDisplay.fromIdAsync(SdkSample.lineDisplayId);
            if (!lineDisplay) {
                WinJS.log("Unable to claim selected LineDisplay from id.", "sample", "error");
            }
        }
        return lineDisplay;
    }

    function parseIntWithFallback(textBox, fallback) {
        var value = parseInt(textBox.value);
        return isNaN(value) ? fallback : value;
    }

    function addOption(select, name, value) {
        var newOption = document.createElement("option");
        newOption.text = name;
        newOption.value = value;
        select.add(newOption);
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        lineDisplayId: "",
        claimScenarioLineDisplayAsync: claimScenarioLineDisplayAsync,
        parseIntWithFallback: parseIntWithFallback,
        addOption: addOption
    });
})();
