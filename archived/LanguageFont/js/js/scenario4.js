//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario4.html", {
        ready: function (element, options) {
            document.getElementById("scenario4Show").addEventListener("click", show, false);
        }
    });

    function show() {
        var output = document.getElementById('scenario4Paragraph');
        var textSpans = output.querySelectorAll('span');

        for (var i = 0, l = textSpans.length; i < l; i++) {
            var text = textSpans[i];

            // Apply font and font size
            var font = WinJS.UI.preferredFonts(text.lang).uiTextFont;
            WinJS.UI.applyFontToElement(text, font);
        }
    }
})();
