//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Globalization = Windows.Globalization;
    var Calendar = Globalization.Calendar;
    var CalendarIdentifiers = Globalization.CalendarIdentifiers;
    var ClockIdentifiers = Globalization.ClockIdentifiers;

    var outputText;

    var page = WinJS.UI.Pages.define("/html/scenario1-data.html", {
        ready: function (element, options) {
            document.getElementById("showResults").addEventListener("click", showResults);
            outputText = document.getElementById("outputText");
        }
    });

    function reportCalendarData(calendar, calendarLabel) {
        var results = calendarLabel + ": " + calendar.getCalendarSystem() + "\n";
        results += "Name of Month: " + calendar.monthAsSoloString() + "\n";
        results += "Day of Month: " + calendar.dayAsPaddedString(2) + "\n";
        results += "Day of Week: " + calendar.dayOfWeekAsSoloString() + "\n";
        results += "Year: " + calendar.yearAsString() + "\n";
        results += "\n";
        return results;
    }

    function showResults() {
        // This scenario uses the Windows.Globalization.Calendar class to display the parts of a date.

        // Create Calendar objects using different constructors.
        var calendar = new Calendar();
        var japaneseCalendar = new Calendar(["ja-JP"], CalendarIdentifiers.japanese, ClockIdentifiers.twelveHour);
        var hebrewCalendar = new Calendar(["he-IL"], CalendarIdentifiers.hebrew, ClockIdentifiers.twentyFourHour);

        // Display the results
        outputText.innerText = 
            reportCalendarData(calendar, "User's default calendar system") +
            reportCalendarData(japaneseCalendar, "Calendar system") +
            reportCalendarData(hebrewCalendar, "Calendar system");
    }
})();
