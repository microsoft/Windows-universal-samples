//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DateTimeFormatting = Windows.Globalization.DateTimeFormatting;
    var DateTimeFormatter = DateTimeFormatting.DateTimeFormatter;

    var page = WinJS.UI.Pages.define("/html/scenario3-parameterizedTemplate.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
        }
    });

    function doDisplay() {
        // This scenario uses the DateTimeFormatter class
        // to format a date/time by specifying a template via parameters.  Note that the current language
        // and region value will determine the pattern of the date returned based on the
        // specified parts.

        SdkSample.clearFormattingResults();
        SdkSample.appendCurrentLanguage();

        // Obtain the date that will be formatted.
        var dateToFormat = new Date();

        SdkSample.appendFormattingResults("Dates:", [
            new DateTimeFormatter(
                DateTimeFormatting.YearFormat.full,
                DateTimeFormatting.MonthFormat.abbreviated,
                DateTimeFormatting.DayFormat.default,
                DateTimeFormatting.DayOfWeekFormat.abbreviated),

            new DateTimeFormatter(
                DateTimeFormatting.YearFormat.abbreviated,
                DateTimeFormatting.MonthFormat.abbreviated,
                DateTimeFormatting.DayFormat.default,
                DateTimeFormatting.DayOfWeekFormat.none),

            new DateTimeFormatter(
                DateTimeFormatting.YearFormat.full,
                DateTimeFormatting.MonthFormat.full,
                DateTimeFormatting.DayFormat.none,
                DateTimeFormatting.DayOfWeekFormat.none),

            new DateTimeFormatter(
                DateTimeFormatting.YearFormat.none,
                DateTimeFormatting.MonthFormat.full,
                DateTimeFormatting.DayFormat.default,
                DateTimeFormatting.DayOfWeekFormat.none)
        ], function (formatter) {
            // Perform the actual formatting.
            return formatter.template + ": " + formatter.format(dateToFormat);
        });

        SdkSample.appendFormattingResults("Times:", [
            new DateTimeFormatter(
                DateTimeFormatting.HourFormat.default,
                DateTimeFormatting.MinuteFormat.default,
                DateTimeFormatting.SecondFormat.default),

            new DateTimeFormatter(
                DateTimeFormatting.HourFormat.default,
                DateTimeFormatting.MinuteFormat.default,
                DateTimeFormatting.SecondFormat.none),

            new DateTimeFormatter(
                DateTimeFormatting.HourFormat.default,
                DateTimeFormatting.MinuteFormat.none,
                DateTimeFormatting.SecondFormat.none)
        ], function (formatter) {
            // Perform the actual formatting.
            return formatter.template + ": " + formatter.format(dateToFormat);
        });
    }
})();
