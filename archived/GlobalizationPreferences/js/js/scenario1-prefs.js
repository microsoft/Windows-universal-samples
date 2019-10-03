//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var GlobalizationPreferences = Windows.System.UserProfile.GlobalizationPreferences;

    var outputText;

    var page = WinJS.UI.Pages.define("/html/scenario1-prefs.html", {
        ready: function (element, options) {
            document.getElementById("showResults").addEventListener("click", showResults);
            outputText = document.getElementById("outputText");
        }
    });

    function showResults() {
        // This scenario uses the Windows.System.UserProfile.GlobalizationPreferences class to
        // obtain the user's globalization preferences.
        var weekDayNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
        outputText.innerText = "Languages: " + GlobalizationPreferences.languages + "\n" +
            "Home Region: " + GlobalizationPreferences.homeGeographicRegion + "\n" +
            "Calendar System: " + GlobalizationPreferences.calendars + "\n" +
            "Clock: " + GlobalizationPreferences.clocks + "\n" +
            "First Day of the Week: " + weekDayNames[GlobalizationPreferences.weekStartsOn];
    }
})();
