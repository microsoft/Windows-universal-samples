//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var content;
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            content = element;
            document.getElementById("scenario2Show").addEventListener("click", show, false);
            document.getElementById("scenario2ItalicHeading").addEventListener("click", italizeHeading, false);
        }
    });

    function show() {
        var output = content.querySelector('.output');
        var heading = output.querySelector('h2');
        var text = output.querySelector('p');

        var font = WinJS.UI.preferredFonts(heading.lang).documentHeadingFont;
        WinJS.UI.applyFontToElement(heading, font);

        // Not all scripts have recommended fonts for "document alternate"
        // categories, so need to verify before using it. Note that Hindi does
        // have document alternate fonts so in this case the fallback logic is
        // unnecessary, but (for example) Japanese does not have recommendations
        // for the document alternate 2 category.
        var preferredFonts = WinJS.UI.preferredFonts(text.lang);
        font = preferredFonts.documentAlternate2Font;
        if (!font) {
            font = preferredFonts.documentAlternate1Font;
        }
        if (!font) {
            font = preferredFonts.modernDocumentFont;
        }
        WinJS.UI.applyFontToElement(text, font);
    }

    function italizeHeading() {
        var heading = document.getElementById('scenario2Heading');
        if (heading.style.fontStyle === 'italic') {
            heading.style.fontStyle = 'normal';
        } else {
            heading.style.fontStyle = 'italic';
        }
    }
})();
