//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DateTimeFormatter = Windows.Globalization.DateTimeFormatting.DateTimeFormatter;

    var page = WinJS.UI.Pages.define("/html/scenario2-stringTemplate.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
        }
    });

    function doDisplay() {
        // This scenario uses the DateTimeFormatter class
        // to format a date/time via a string template.  Note that the order specifed in the string pattern does
        // not determine the order of the parts of the formatted string.  The current language and region value will
        // determine the pattern of the date returned based on the specified parts. 

        SdkSample.clearFormattingResults();
        SdkSample.appendCurrentLanguage();

        // Obtain the date that will be formatted.
        var dateToFormat = new Date();

        SdkSample.appendFormattingResults("Dates:", [
            new DateTimeFormatter("month day"),
            new DateTimeFormatter("month year"),
            new DateTimeFormatter("month day year"),
            new DateTimeFormatter("month day dayofweek year"),
            new DateTimeFormatter("dayofweek.abbreviated"),
            new DateTimeFormatter("month.abbreviated"),
            new DateTimeFormatter("year.abbreviated")
        ], function (formatter) {
            // Perform the actual formatting.
            return formatter.template + ": " + formatter.format(dateToFormat);
        });

        SdkSample.appendFormattingResults("Times:", [
            new DateTimeFormatter("hour minute"),
            new DateTimeFormatter("hour minute second"),
            new DateTimeFormatter("hour")
        ], function (formatter) {
            // Perform the actual formatting.
            return formatter.template + ": " + formatter.format(dateToFormat);
        });

        SdkSample.appendFormattingResults("Time zones:", [
            new DateTimeFormatter("timezone"),
            new DateTimeFormatter("timezone.full"),
            new DateTimeFormatter("timezone.abbreviated")
        ], function (formatter) {
            // Perform the actual formatting.
            return formatter.template + ": " + formatter.format(dateToFormat);
        });

        SdkSample.appendFormattingResults("Date and time combinations:", [
            new DateTimeFormatter("hour minute second timezone.full"),
            new DateTimeFormatter("day month year hour minute timezone"),
            new DateTimeFormatter("dayofweek day month year hour minute second"),
            new DateTimeFormatter("dayofweek.abbreviated day month hour minute"),
            new DateTimeFormatter("dayofweek day month year hour minute second timezone.abbreviated")
        ], function (formatter) {
            // Perform the actual formatting.
            return formatter.template + ": " + formatter.format(dateToFormat);
        });
    }
})();
