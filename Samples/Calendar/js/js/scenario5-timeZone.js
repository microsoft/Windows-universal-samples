//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Calendar = Windows.Globalization.Calendar;

    var outputText;

    var page = WinJS.UI.Pages.define("/html/scenario5-timeZone.html", {
        ready: function (element, options) {
            document.getElementById("showResults").addEventListener("click", showResults);
            outputText = document.getElementById("outputText");
        }
    });

    function reportCalendarData(calendar) {
        // Display individual date/time elements.
        return "In " + calendar.getTimeZone() +
            " time zone:   " + calendar.dayOfWeekAsSoloString() +
            "   " + calendar.monthAsSoloString() +
            " " + calendar.dayAsPaddedString(2) +
            ", " + calendar.yearAsString() +
            "   " + calendar.hourAsPaddedString(2) +
            ":" + calendar.minuteAsPaddedString(2) +
            ":" + calendar.secondAsPaddedString(2) +
            " " + calendar.periodAsString() +
            "  " + calendar.timeZoneAsString(3) +
            "\n";
    }

    function showResults() {
        // This scenario illustrates time zone support in Windows.Globalization.Calendar class

        // Displayed time zones in addition to the local time zone.
        var timeZones = ["UTC", "America/New_York", "Asia/Kolkata"];

        // Create default Calendar object
        var calendar = new Calendar();
        var localTimeZone = calendar.getTimeZone();

        // Show current time in local time zone
        var results = "Current date and time:\n";
        results += reportCalendarData(calendar);

        // Show current time in additional time zones
        timeZones.forEach(function (timeZone) {
            calendar.changeTimeZone(timeZone);
            results += reportCalendarData(calendar);
        });
        results += "\n";

        // Change back to local time zone
        calendar.changeTimeZone(localTimeZone);

        // Show a time on 14th day of second month of next year.
        // Note the effect of daylight saving on the results.
        results += "Same time on 14th day of second month of next year:\n";
        calendar.addYears(1); calendar.month = 2; calendar.day = 14;
        results += reportCalendarData(calendar);
        timeZones.forEach(function (timeZone) {
            calendar.changeTimeZone(timeZone);
            results += reportCalendarData(calendar);
        });
        results += "\n";

        // Change back to local time zone
        calendar.changeTimeZone(localTimeZone);

        // Show a time on 14th day of tenth month of next year.
        // Note the effect of daylight saving time on the results.
        results += "Same time on 14th day of tenth month of next year:\n";
        calendar.addMonths(8);
        results += reportCalendarData(calendar);
        timeZones.forEach(function (timeZone) {
            calendar.changeTimeZone(timeZone);
            results += reportCalendarData(calendar);
        });

        // Display the results
        outputText.innerText = results;
    }
})();
