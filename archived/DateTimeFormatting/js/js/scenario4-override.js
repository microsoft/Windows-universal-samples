//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DateTimeFormatting = Windows.Globalization.DateTimeFormatting;
    var DateTimeFormatter = DateTimeFormatting.DateTimeFormatter;
    var CalendarIdentifiers = Windows.Globalization.CalendarIdentifiers;
    var ClockIdentifiers = Windows.Globalization.ClockIdentifiers;

    var page = WinJS.UI.Pages.define("/html/scenario4-override.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
        }
    });

    function doDisplay() {
        // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
        // to format a date/time by using a formatter that provides specific languages,
        // geographic region, calendar and clock

        SdkSample.clearFormattingResults();
        SdkSample.appendCurrentLanguage();

        // Obtain the date that will be formatted.
        var dateToFormat = new Date();

        // Formatters for dates, using basic formatters, string templates and parametrized templates.
        SdkSample.appendFormattingResults("Dates:", [
            new DateTimeFormatter("longdate", ["ja-JP"], "JP", CalendarIdentifiers.japanese, ClockIdentifiers.twelveHour),
            new DateTimeFormatter("month day", ["fr-FR"], "FR", CalendarIdentifiers.gregorian, ClockIdentifiers.twentyFourHour),
            new DateTimeFormatter(
                DateTimeFormatting.YearFormat.abbreviated,
                DateTimeFormatting.MonthFormat.abbreviated,
                DateTimeFormatting.DayFormat.default,
                DateTimeFormatting.DayOfWeekFormat.none,
                DateTimeFormatting.HourFormat.none,
                DateTimeFormatting.MinuteFormat.none,
                DateTimeFormatting.SecondFormat.none,
                ["de-DE"], "DE",
                CalendarIdentifiers.gregorian, ClockIdentifiers.twelveHour)
        ], function (formatter) {
            // Perform the actual formatting.
            return formatter.template + ": (" + formatter.resolvedLanguage + ") " + formatter.format(dateToFormat);
        });

        // Formatters for times, using basic formatters, string templates and parametrized templates.
        SdkSample.appendFormattingResults("Times:", [
            new DateTimeFormatter("longtime", ["ja-JP"], "JP", CalendarIdentifiers.japanese, ClockIdentifiers.twelveHour),
            new DateTimeFormatter("hour minute", ["fr-FR"], "FR", CalendarIdentifiers.gregorian, ClockIdentifiers.twentyFourHour),
            new DateTimeFormatter(
                DateTimeFormatting.YearFormat.none,
                DateTimeFormatting.MonthFormat.none,
                DateTimeFormatting.DayFormat.none,
                DateTimeFormatting.DayOfWeekFormat.none,
                DateTimeFormatting.HourFormat.default,
                DateTimeFormatting.MinuteFormat.default,
                DateTimeFormatting.SecondFormat.none,
                ["de-DE"], "DE",
                CalendarIdentifiers.gregorian, ClockIdentifiers.twelveHour)
        ], function (formatter) {
            // Perform the actual formatting.
            return formatter.template + ": (" + formatter.resolvedLanguage + ") " + formatter.format(dateToFormat);
        });
    }
})();
