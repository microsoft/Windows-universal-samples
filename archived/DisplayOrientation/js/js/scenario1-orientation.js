//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Convenient aliases for WinRT types.
    var DisplayInformation = Windows.Graphics.Display.DisplayInformation;
    var DisplayOrientations = Windows.Graphics.Display.DisplayOrientations;

    // Elements in the HTML page.
    var nativeOrientationSpan;
    var currentOrientationSpan;
    var autoRotationPreferencesSpan;
    var preferLandscape;
    var preferPortrait;
    var preferLandscapeFlipped;
    var preferPortraitFlipped;

    // Per-page state.
    var displayInfo;

    var page = WinJS.UI.Pages.define("/html/scenario1-orientation.html", {
        ready: function (element, options) {
            nativeOrientationSpan = document.getElementById("nativeOrientationSpan");
            currentOrientationSpan = document.getElementById("currentOrientationSpan");
            autoRotationPreferencesSpan = document.getElementById("autoRotationPreferencesSpan");
            preferLandscape = document.getElementById("preferLandscape");
            preferPortrait = document.getElementById("preferPortrait");
            preferLandscapeFlipped = document.getElementById("preferLandscapeFlipped");
            preferPortraitFlipped = document.getElementById("preferPortraitFlipped");

            document.getElementById("applyRotationLock").addEventListener("click", applyRotationLock, false);

            displayInfo = DisplayInformation.getForCurrentView();
            displayInfo.addEventListener("orientationchanged", updateContent, false);

            updateContent();
        },

        unload: function () {
            displayInfo.removeEventListener("orientationchanged", updateContent);
        }
    });

    function orientationsToString(orientations) {
        var flags = [];
        if (orientations & DisplayOrientations.landscape)
        {
            flags.push("Landscape");
        }
        if (orientations & DisplayOrientations.portrait)
        {
            flags.push("Portrait");
        }
        if (orientations & DisplayOrientations.landscapeFlipped)
        {
            flags.push("LandscapeFlipped");
        }
        if (orientations & DisplayOrientations.portraitFlipped)
        {
            flags.push("PortraitFlipped");
        }
        return flags.join(", ") || "None";
    }

    function updateContent()
    {
        nativeOrientationSpan.innerText = orientationsToString(displayInfo.nativeOrientation);
        currentOrientationSpan.innerText = orientationsToString(displayInfo.currentOrientation);
        autoRotationPreferencesSpan.innerText = orientationsToString(DisplayInformation.autoRotationPreferences);

    }

    function applyRotationLock()
    {
        var orientations = DisplayOrientations.none;
        if (preferLandscape.checked)
        {
            orientations |= DisplayOrientations.landscape;
        }
        if (preferPortrait.checked)
        {
            orientations |= DisplayOrientations.portrait;
        }
        if (preferLandscapeFlipped.checked)
        {
            orientations |= DisplayOrientations.landscapeFlipped;
        }
        if (preferPortraitFlipped.checked)
        {
            orientations |= DisplayOrientations.portraitFlipped;
        }
        DisplayInformation.autoRotationPreferences = orientations;
        updateContent();

    }
})();
