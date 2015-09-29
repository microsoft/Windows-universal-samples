//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Globalization = Windows.Globalization;
    var Calendar = Globalization.Calendar;
    var CalendarIdentifiers = Globalization.CalendarIdentifiers;
    var ClockIdentifiers = Globalization.ClockIdentifiers;
    var DateTimeFormatter = Globalization.DateTimeFormatting.DateTimeFormatter;

    var outputText;

    var page = WinJS.UI.Pages.define("/html/scenario3-enum.html", {
        ready: function (element, options) {
            document.getElementById("showResults").addEventListener("click", showResults);
            outputText = document.getElementById("outputText");
        }
    });

    function showResults() {
        // This scenario uses the Windows.Globalization.Calendar class to enumerate through a calendar and
        // perform calendar math
        var results = "The number of years in each era of the Japanese era calendar is not regular. " +
                      "It is determined by the length of the given imperial era:\n";

        // Create Japanese calendar.
        var calendar = new Calendar(["en-US"], CalendarIdentifiers.japanese, ClockIdentifiers.twentyFourHour);

        // Enumerate all supported years in all supported Japanese eras.
        for (calendar.era = calendar.firstEra; true; calendar.addYears(1)) {
            // Process current era.
            results += "Era " + calendar.eraAsString() + " contains " + calendar.numberOfYearsInThisEra + " year(s)\n";

            // Enumerate all years in this era.
            for (calendar.year = calendar.firstYearInThisEra; true; calendar.addYears(1)) {
                // Begin sample processing of current year.

                // Move to first day of year. Change of month can affect day so order of assignments is important.
                calendar.month = calendar.firstMonthInThisYear;
                calendar.day = calendar.firstDayInThisMonth;

                // Set time to midnight (local).
                calendar.period = calendar.firstPeriodInThisDay;    // All days have 1 or 2 periods depending on clock type
                calendar.hour = calendar.firstHourInThisPeriod;     // Hours start from 12 or 0 depending on clock type
                calendar.minute = 0;
                calendar.second = 0;
                calendar.nanosecond = 0;

                if (calendar.year % 1000 == 0) {
                    results += "\n";
                }
                else if (calendar.year % 10 == 0) {
                    results += ".";
                }

                // End sample processing of current year.

                // Break after processing last year.
                if (calendar.year == calendar.lastYearInThisEra) {
                    break;
                }
            }
            results += "\n";

            // Break after processing last era.
            if (calendar.era == calendar.lastEra) {
                break;
            }
        }
        results += "\n";

        // This section shows enumeration through the hours in a day to demonstrate that the number of time units in a given period (hours in a day, minutes in an hour, etc.)
        // should not be regarded as fixed. With Daylight Saving Time and other local calendar adjustments, a given day may have not have 24 hours, and
        // a given hour may not have 60 minutes, etc.
        results += "The number of hours in a day is not constant. " +
                   "The US calendar transitions from daylight saving time to standard time on 4 November 2012:\n\n";

        // Create a DateTimeFormatter to display dates
        var displayDate = new DateTimeFormatter("longdate");

        // Create a gregorian calendar for the US with 12-hour clock format
        var currentCal = new Calendar(["en-US"], CalendarIdentifiers.gregorian, ClockIdentifiers.twentyFourHour, "America/Los_Angeles");

        // Set the calendar to a the date of the Daylight Saving Time-to-Standard Time transition for the US in 2012.
        // DST ends in the America/Los_Angeles time zone at 4 November 2012 02:00 PDT = 4 November 2012 09:00 UTC.
        // (Note that month numbers start at zero in JavaScript, so month 10 is the eleventh month.)
        var dstDate = new Date(Date.UTC(2012, 10, 4, 9, 0, 0));
        currentCal.setDateTime(dstDate);

        // Set the current calendar to one day before DST change. Create a second calendar for comparision and set it to one day after DST change.
        var endDate = currentCal.clone();
        currentCal.addDays(-1);
        endDate.addDays(1);

        // Enumerate the day before, the day of, and the day after the 2012 DST-to-Standard time transition
        while (currentCal.day <= endDate.day) {
            // Process current day.
            var date = currentCal.getDateTime();
            results += displayDate.format(date) + " contains " + currentCal.numberOfHoursInThisPeriod + " hour(s)\n";

            // Enumerate all hours in this day.
            // Create a calendar to represent the following day.
            var nextDay = currentCal.clone();
            nextDay.addDays(1);
            for (currentCal.hour = currentCal.firstHourInThisPeriod; true; currentCal.addHours(1)) {
                // Display the hour for each hour in the day.             
                results += currentCal.hourAsPaddedString(2) + " ";

                // Break upon reaching the next period (i.e. the first period in the following day).
                if (currentCal.day == nextDay.day && currentCal.period == nextDay.period) {
                    break;
                }
            }
            results += "\n";
        }

        // Display results
        outputText.innerText = results;
    }
})();
