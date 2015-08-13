//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario6-timeZone.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
        }
    });

    function doDisplay() {
        // This scenario illustrates time zone support in DateTimeFormatter class
        SdkSample.clearFormattingResults();

        // Additional time zones we will show.
        var timeZones = ["UTC", "America/New_York", "Asia/Kolkata"];
        
        // Create formatter object using longdate and longtime template
        var formatter = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter("longdate longtime");
        
        // Obtain the date that will be formatted.
        var dateToFormat = new Date();
        
        // Show current time.
        var results = "Current date and time\n";
        results += "In Local time zone: " + formatter.format(dateToFormat) + "\n";
        timeZones.forEach(function (timeZone) {
            results += "In " + timeZone + " time zone: " + formatter.format(dateToFormat, timeZone) + "\n";
        });
        results += "\n";

        // Show a time on 14th day of second month of next year.
        // (Note that month numbers start at zero in JavaScript, so month 1 is the second month.)
        // Note the effect of daylight saving on the results.
        results += "Same time on 14th day of second month of next year\n";
        dateToFormat = new Date(dateToFormat.getFullYear() + 1, 1, 14, dateToFormat.getHours(), dateToFormat.getMinutes(), dateToFormat.getSeconds());
        results += "In local time zone: " + formatter.format(dateToFormat) + "\n";
        timeZones.forEach(function (timeZone) {
            results += "In " + timeZone + " time zone: " + formatter.format(dateToFormat, timeZone) + "\n";
        });
        results += "\n";

        // Show a time on 14th day of tenth month of next year.
        // Note the effect of daylight saving on the results.
        results += "Same time on 14th day of tenth month of next year\n";
        dateToFormat.setMonth(9);
        results += "In local time zone: " + formatter.format(dateToFormat) + "\n";
        timeZones.forEach(function (timeZone) {
            results += "In " + timeZone + " time zone: " + formatter.format(dateToFormat, timeZone) + "\n";
        });
        results += "\n";

        // Display the results.
        document.getElementById("output").innerText = results;
    }
})();
