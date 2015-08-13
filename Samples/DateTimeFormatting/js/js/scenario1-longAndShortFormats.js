//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DateTimeFormatter = Windows.Globalization.DateTimeFormatting.DateTimeFormatter;

    var page = WinJS.UI.Pages.define("/html/scenario1-longAndShortFormats.html", {
        ready: function (element, options) {
            document.getElementById("displayButton").addEventListener("click", doDisplay, false);
        }
    });

    function doDisplay() {

        // This scenario uses the DateTimeFormatting.DateTimeFormatter class
        // in order to display dates and times using basic formatters.  Note that the pattern of 
        // the date returned is determined by the current language and region value of the application context

        SdkSample.clearFormattingResults();
        SdkSample.appendCurrentLanguage();

        // Obtain the date that will be formatted.
        var dateToFormat = new Date();

        SdkSample.appendFormattingResults("Formatted results:", [
            // Formatters for dates.
            new DateTimeFormatter("shortdate"),
            new DateTimeFormatter("longdate"),

            // Formatters for times.
            new DateTimeFormatter("shorttime"),
            new DateTimeFormatter("longtime")

        ], function (formatter) {
            // Perform the actual formatting.
            return formatter.template + ": " + formatter.format(dateToFormat);
        });
    }
})();
