//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var content;
    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            content = element;
            document.getElementById("scenario3Show").addEventListener("click", show, false);
            document.getElementById("scenario3Select").addEventListener("change", change, false);
            document.getElementById("scenario3Reset").addEventListener("click", reset, false);
        }
    });

    function show() {
        var output = content.querySelector('.output');
        var paragraphs = output.querySelectorAll('p');
        var select = document.getElementById('scenario3Select');

        for (var p = 0, n = paragraphs.length; p < n; ++p) {
            var paragraph = paragraphs[p];
            var textSpans = paragraph.querySelectorAll('span');
            var baseFont = WinJS.UI.preferredFonts(textSpans[0].lang).uiTextFont;
            for (var i = 0, l = textSpans.length; i < l; i++) {
                var text = textSpans[i];

                // Apply font and font size.  The base text is to be drawn at our selected size.  The
                // dependent text must be scaled; the appropriate scale factor is calculated by noting
                // that both the base font and the dependent font have a scaling factor that is
                // used to scale the font relative to the standard Latin font.
                var font = WinJS.UI.preferredFonts(text.lang).uiTextFont;
                var fontSize = select.value * (font.scaleFactor / baseFont.scaleFactor);

                text.style.fontSize = /*@static_cast(String)*/ fontSize + "pt";
                WinJS.UI.applyFontToElement(text, font);
            }
        }
    }

    function change() {
        var output = content.querySelector('.output');
        var textSpans = output.querySelectorAll('span');
        var select = document.getElementById('scenario3Select');

        for (var i = 0, l = textSpans.length; i < l; i++) {
            var text = textSpans[i];
            var font = WinJS.UI.preferredFonts(text.lang).uiTextFont;

            text.style.fontSize = select.value + "pt";
            WinJS.UI.applyFontToElement(text, font);
        }
    }

    function reset() {
        var output = content.querySelector('.output');
        var textSpans = output.querySelectorAll('span');
        var select = document.getElementById('scenario3Select');

        for (var i = 0, l = textSpans.length; i < l; i++) {
            var text = textSpans[i];
            var font = WinJS.UI.preferredFonts(text.lang).uiTextFont;

            text.setAttribute("style", "");
            text.style.fontSize = select.value + "pt";
            WinJS.UI.applyFontToElement(text, font);
        }

    }
})();
