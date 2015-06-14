//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/referencingPackagedContent.html", {
        ready: function (element, options) {
            var fullyQualifiedImageSource = "ms-appx://" + document.location.host + "/images/officeLogo.png";
            document.getElementById("fullyQualifiedImage").src = fullyQualifiedImageSource;
            document.getElementById("fullyQualifiedImageReference").innerText = fullyQualifiedImageSource;
        }
    });
})();
