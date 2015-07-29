//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    
    var content;
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            content = element;
            document.getElementById("scenario1Apply").addEventListener("click", apply, false);
        }
    });

    function apply() {
        var output = content.querySelector('.output');
        var heading = output.querySelector('h2');
        var text = output.querySelector('p');

        // The HTML lang attribute is set appropriately on the elements
        // WinJS.UI font APIS can be found in langfont.js
        var font = WinJS.UI.preferredFonts(heading.lang).uiHeadingFont;
        WinJS.UI.applyFontToElement(heading, font);

        font = WinJS.UI.preferredFonts(text.lang).uiTextFont;
        WinJS.UI.applyFontToElement(text, font);
    }
})();
