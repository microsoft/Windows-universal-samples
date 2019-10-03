//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Globalization = Windows.Globalization;
    var Calendar = Globalization.Calendar;
    var CalendarIdentifiers = Globalization.CalendarIdentifiers;
    var ClockIdentifiers = Globalization.ClockIdentifiers;

    var outputText;

    var page = WinJS.UI.Pages.define("/html/scenario2-stats.html", {
        ready: function (element, options) {
            document.getElementById("showResults").addEventListener("click", showResults);
            outputText = document.getElementById("outputText");
        }
    });

    function reportCalendarData(calendar, calendarLabel) {
        var results = calendarLabel + ": " + calendar.getCalendarSystem() + "\n";
        results += "Months in this Year: " + calendar.numberOfMonthsInThisYear + "\n";
        results += "Days in this Month: " + calendar.numberOfDaysInThisMonth + "\n";
        results += "Hours in this Period: " + calendar.numberOfHoursInThisPeriod + "\n";
        results += "Era: " + calendar.eraAsString() + "\n";
        results += "\n";
        return results;
    }

    function showResults() {
        // This scenario uses the Windows.Globalization.Calendar class to display the calendar
        // system statistics.

        // Create Calendar objects using different constructors.
        var calendar = new Calendar();
        var japaneseCalendar = new Calendar(["ja-JP"], CalendarIdentifiers.japanese, ClockIdentifiers.twelveHour);
        var hebrewCalendar = new Calendar(["he-IL"], CalendarIdentifiers.hebrew, ClockIdentifiers.twentyFourHour);

    // Display results
    outputText.innerText =
        reportCalendarData(calendar, "User's default calendar system") +
        reportCalendarData(japaneseCalendar, "Calendar system") +
        reportCalendarData(hebrewCalendar, "Calendar system");
    }
})();
