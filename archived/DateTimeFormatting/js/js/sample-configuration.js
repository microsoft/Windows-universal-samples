//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Date and time formatting";

    var scenarios = [{
        url: "/html/scenario1-longAndShortFormats.html",
        title: "Format date and time using long and short"
    }, {
        url: "/html/scenario2-stringTemplate.html",
        title: "Format using a string template"
    }, {
        url: "/html/scenario3-parameterizedTemplate.html",
        title: "Format using a parameterized template"
    }, {
        url: "/html/scenario4-override.html",
        title: "Override the current user's settings"
    }, {
        url: "/html/scenario5-unicodeExtensions.html",
        title: "Formatters using Unicode extensions"
    }, {
        url: "/html/scenario6-timeZone.html",
        title: "Format using different time zones"
    }];

    function clearFormattingResults() {
        document.getElementById("output").innerText = "";
    }

    function appendSection(text) {
        var section = document.createElement("p");
        section.innerText = text;
        document.getElementById("output").appendChild(section);
    }

    function appendCurrentLanguage() {
        var currentLanguage = Windows.Globalization.ApplicationLanguages.languages[0];
        appendSection("Current application context language: " + currentLanguage);
    }

    function appendFormattingResults(title, formatters, generator) {
        var results = title + "\n";
        formatters.forEach(function (formatter) {
            results += generator(formatter) + "\n";
        });
        appendSection(results);
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        clearFormattingResults: clearFormattingResults,
        appendCurrentLanguage: appendCurrentLanguage,
        appendFormattingResults: appendFormattingResults
    });
})();