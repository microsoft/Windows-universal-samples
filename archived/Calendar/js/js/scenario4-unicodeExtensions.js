//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var Calendar = Windows.Globalization.Calendar;

    var outputText;

    var page = WinJS.UI.Pages.define("/html/scenario4-unicodeExtensions.html", {
        ready: function (element, options) {
            document.getElementById("showResults").addEventListener("click", showResults);
            outputText = document.getElementById("outputText");
        }
    });
    
    function reportCalendarData(calendar, calendarLabel) {
        var results = calendarLabel + ": " + calendar.getCalendarSystem() + "\n";
        results += "Calendar system: " + calendar.getCalendarSystem() + "\n";
        results += "Numeral System: " + calendar.numeralSystem + "\n";
        results += "Resolved Language " + calendar.resolvedLanguage + "\n";
        results += "Name of Month: " + calendar.monthAsSoloString() + "\n";
        results += "Day of Month: " + calendar.dayAsPaddedString(2) + "\n";
        results += "Day of Week: " + calendar.dayOfWeekAsSoloString() + "\n";
        results += "Year: " + calendar.yearAsString() + "\n";
        results += "\n";
        return results;
    }

    function showResults() {
        // This scenario uses the Windows.Globalization.Calendar class to display the parts of a date.

        // Create Calendar objects using different Unicode extensions for different languages.
        // NOTE: Calendar (ca) and numeral system (nu) are the only supported extensions with any others being ignored.
        // Note that collation (co) extension is ignored in the last example.
        var cal1 = new Calendar();
        var cal2 = new Calendar(["ar-SA-u-ca-gregory-nu-Latn"]);
        var cal3 = new Calendar(["he-IL-u-nu-arab"]);
        var cal4 = new Calendar(["he-IL-u-ca-hebrew-co-phonebk"]);

        // Display individual date/time elements.
        outputText.innerText =
            reportCalendarData(cal1, "User's default Calendar object") +
            reportCalendarData(cal2, "Calendar object with Arabic language, Gregorian Calendar and Latin Numeral System (ar-SA-ca-gregory-nu-Latn)") +
            reportCalendarData(cal3, "Calendar object with Hebrew language, Default Calendar for that language and Arab Numeral System (he-IL-u-nu-arab)") +
            reportCalendarData(cal4, "Calendar object with Hebrew language, Hebrew Calendar, Default Numeral System for that language and Phonebook collation (he-IL-u-ca-hebrew-co-phonebk)");
    }
})();
