//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DateTimeFormatting = Windows.Globalization.DateTimeFormatting;
    var DateTimeFormatter = DateTimeFormatting.DateTimeFormatter;

    var page = WinJS.UI.Pages.define("/html/scenario5-unicodeExtensions.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
        }
    });

    function doDisplay() {
        // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
        // to format a date/time by using a formatter that uses unicode extenstion in the specified
        // language name

        SdkSample.clearFormattingResults();
        SdkSample.appendCurrentLanguage();

        // Obtain the date that will be formatted.
        var dateToFormat = new Date();

        SdkSample.appendFormattingResults("Dates and times:", [
            // Create formatters using various types of constructors specifying Language list with Unicode extension in language names

            // Default application context 
            new DateTimeFormatter("longdate longtime"),

            // Telugu language, Gregorian Calendar and Latin Numeral System
            new DateTimeFormatter("longdate longtime", ["te-in-u-ca-gregory-nu-latn", "en-US"]),

            // Hebrew language and Arabic Numeral System - calendar NOT specified in constructor
            new DateTimeFormatter(
                DateTimeFormatting.YearFormat.default,
                DateTimeFormatting.MonthFormat.default,
                DateTimeFormatting.DayFormat.default,
                DateTimeFormatting.DayOfWeekFormat.default,
                DateTimeFormatting.HourFormat.default,
                DateTimeFormatting.MinuteFormat.default,
                DateTimeFormatting.SecondFormat.default,
                ["he-IL-u-nu-arab", "en-US"]),

            // Hebrew language and calendar - calendar specified in constructor
            // also, which overrides the one specified in Unicode extension
            new DateTimeFormatter(
                DateTimeFormatting.YearFormat.default,
                DateTimeFormatting.MonthFormat.default,
                DateTimeFormatting.DayFormat.default,
                DateTimeFormatting.DayOfWeekFormat.default,
                DateTimeFormatting.HourFormat.default,
                DateTimeFormatting.MinuteFormat.default,
                DateTimeFormatting.SecondFormat.default,
                ["he-IL-u-ca-hebrew-co-phonebk", "en-US"],
                "US",
                Windows.Globalization.CalendarIdentifiers.gregorian,
                Windows.Globalization.ClockIdentifiers.twentyFourHour)
        ], function (formatter) {
            // Perform the actual formatting.
            return "Result: " + formatter.format(dateToFormat) + "\n" +
                    "    Language List: " + formatter.languages.join(", ") + "\n" +
                    "    Template: " + formatter.template + "\n" +
                    "    Resolved Language: " + formatter.resolvedLanguage + "\n";
                    "    Calendar System: " + formatter.calendar + "\n";
                    "    Numeral System: " + formatter.numeralSystem + "\n";
        });
        return;

    }
})();
